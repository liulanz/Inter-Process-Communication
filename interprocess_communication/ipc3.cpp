#include <stdio.h> 
#include <sys/wait.h>  
#include <iostream>
#include <unistd.h>
#include <mutex>
#include <sys/shm.h> 
#include <semaphore.h>
#include <fcntl.h>
#include <string>
#include <vector>
using namespace std;

#define GRAD_READ pipeML3D[0]
#define GRAD_WRITE pipeML3D[1]
#define D3_READ pipe3DML[0]
#define D3_WRITE pipe3DML[1]

/* list of processes that will be run */
vector<string> PROGRAMS = {"./w1.out","./w2.out", "./w3.out", "./rscode"};

int pipeML3D[2], pipe3DML[2];
mutex m;
/* process queue */
struct pcb{
    pid_t pid;
    pcb* next;
};
pcb * head;

void Enqueue(pcb* p){
    pcb* curr;
    m.lock();
    curr = head;
    if(curr == NULL)
        head = p;
    else {
        while(curr->next!=NULL){
            curr= curr->next;
        }
        curr->next = p;
    }
    p->next = NULL;
    m.unlock();
}

pcb* Dequeue(){
    int sval;
    pcb * front;
    m.lock();
    front = head;
    if(front != NULL){
        head = front->next;
    }
    m.unlock();
    return front;
}

/* Program sending data through pipe */
void Send(const char * program1){
    close(GRAD_READ);
    close(D3_READ);
    close(D3_WRITE);
    cout << "[" << program1 << "] will send data" << endl;
    dup2(GRAD_WRITE , STDOUT_FILENO); /* make output go to pipe */
    execlp(program1, program1, NULL);
}

void Receive(const char *  program2){
    cout << "[" << program2 << "] will receive data" << endl;
    dup2(GRAD_READ,STDIN_FILENO); /* get input from pipe */
    close(GRAD_WRITE);
    close(D3_READ);
    close(D3_WRITE);
    execlp(program2, program2, NULL);
}

int main(int argc, char* argv[]){
	sem_t *sem; 
	int *process_num;
    int shmid;  
    head = NULL;
    int i;
    int status;     
    pid_t pid;
    int sem_num = 2;    /* binary semaphore */
    shmid = shmget (IPC_PRIVATE, sizeof (int), IPC_CREAT | 0666);
    if (shmid < 0) {                           /* shmget error check */
        cout << "shmget error" << endl;
        exit (1);
    }
    
    /* Attach shared variable to shared memory using shmat  */
    process_num = (int *) shmat (shmid, NULL, 0);   
    if(*process_num == -1){
        cout << "Attachment errr" << endl;
        exit(1);
    }
    else
        cout << "[Successfully attached shared variable to shared memory]" << endl;
    sem = sem_open ("write_sem", O_CREAT, 0644, sem_num);
    
    *process_num = 0;
	
    /* Creating pipes */
	if(pipe(pipeML3D) == -1 ){  
        perror("Pipe creation failure.");
        exit(EXIT_FAILURE);
    }
    if(pipe(pipe3DML) == -1 ){  
        perror("Pipe creation failure.");
        exit(EXIT_FAILURE);
    }

    /* Forking processes */
    for (i = 0; i < 4; i++){
        pid = fork();
        if (pid < 0) {       /* Error check */    
            cout << "fork error" << endl;
            exit(1);
        }
        else if (pid == 0)  /* Child process */
            break;   
        else{
            pcb* process = new pcb {pid, NULL};
            kill(pid, SIGSTOP);

            Enqueue(process);

        }               
    }
    if (pid > 0){ /* Parent Process */ 

        while (head != NULL){
        	kill(head->pid, SIGCONT);
            int endID = waitpid(head->pid, &status, WNOHANG|WUNTRACED);
            if(endID == -1){
                perror("waitpid error");
                exit(1);
            }
            else if(endID == head->pid){
           
                Dequeue();
            }
            else{
            	sleep(1);
            	if(*process_num==4){
            		sleep(1);
	            	close(GRAD_READ);
				    close(GRAD_WRITE);
				    close(D3_READ);
				    close(D3_WRITE);
				}
            }
        }

        /* shared memory detach  */
        int status= shmdt (process_num);
        if(status == -1){
            cout << "Error: Detach shared memory segment  " << endl;
            exit(1);
        }
        else
            cout << "[Successfully detach shared memory segment]." << endl;
        status = shmctl (shmid, IPC_RMID, 0); /* Remove segment */
        if(status == -1){
            cout << "Error: Remove shared memory segment  " << endl;
            exit(1);
        }
        else
            cout << "[Successfully removed shared memory segment].\n " << endl;

        /* cleanup semaphores */
        sem_destroy (sem);
        exit(0);
      
    }
    else{   /* Child process */  
    
         sem_trywait (sem);
         (*process_num)++;
         sem_post (sem);  
   		string str= PROGRAMS[*process_num-1];
        const char *cstr = str.c_str();
        if(*process_num == 4){
        	Receive(cstr);
        }
        else{
        	Send(cstr);
        }

    }

  
}
   

