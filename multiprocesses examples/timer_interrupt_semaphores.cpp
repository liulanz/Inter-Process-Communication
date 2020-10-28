/* 
    This program has a semaphore value of 3
    It uses counthing semaphore to let the processes enter the shared resource
    It uses semaphore and mutex to protect the shared resource, so only one can modify the shared resource at a time
    
    This program also implements external interrupt with a timer signal
    When there's a timer interrupt, the current running process will be pause and move to the back of the queue, and the next 
    child process will start running.
*/

#include <stdlib.h>         /* exit(), malloc(), free() */
#include <sys/types.h>      /* key_t, sem_t, pid_t      */
#include <sys/shm.h>        /* shmat(), IPC_RMID        */
#include <errno.h>          /* errno, ECHILD            */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>          /* O_CREAT, O_EXEC          */
#include <unistd.h> 
#include <sys/wait.h>       
#include <iostream>
#include <mutex>
#include <signal.h>
#include <queue>
using namespace std;

#define SEM_NUM 3

struct shared_resource{
    int value;
};
struct pcb{
    pid_t pid;
    pcb* next;
};
sem_t *sem; 
pcb * head; 
mutex m_running;
mutex m;
mutex m_head;

void Enqueue(pcb* p){
    pcb* curr;
    m.lock();
    curr = head;
    if(curr == NULL){
        head = p;
        
    }
    else
    {
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
void sigalrm_handler(int sig){
    lock_guard<mutex> locker (m_head);
    if(head!=NULL){
        pcb* running = Dequeue();
        kill(running->pid, SIGSTOP);
        cout <<" TIMER INTERRUPT !!! pause " << running->pid<<endl;
        Enqueue(running);
        kill(head->pid, SIGCONT);
        alarm(2);
    }      
}
int main ( ){
    head = NULL;
    int shmid;                                  
    pid_t pid;                    
    shared_resource *glob;
    int fork_n;               
    int i;
    time_t t;
    int status;
    cout << "===================================================" << endl;
    /*  
        Creating shared memory for int, 
        IPC_PRIVATE: a uniquely generated key 
        http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/shm/shmget.html
    */
    shmid = shmget (IPC_PRIVATE, sizeof (struct shared_resource), IPC_CREAT | 0666);
    if (shmid < 0) {                           /* shmget error check */
        cout << "shmget error" << endl;
        exit (1);
    }
    /*
        Attach shared variable to shared memory using shmat 
        http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/shm/shmat.html
    */
    glob = (shared_resource *) shmat (shmid, NULL, 0);   
    
    if(glob == (void *) -1){
        printf("Error attaching shared memory");
        exit(1);
    }
    else
        cout << "[Successfully attach shared resource to shared memory]" <<endl;
    
    /* 
        initialize semaphores for shared processes 
        sem_t *sem = sem_open(SNAME, O_CREAT, 0644, 3);
        sem_t *sem_open(const char, *name, int oflag, mode_t mode, unsigned int value);
    */
    sem = sem_open ("write_sem", O_CREAT, 0644, SEM_NUM);
     sem_init(sem, 1, SEM_NUM);
    if(sem == (void *) -1){
        printf("Error attaching semaphore to shared memory");
        exit(1);
    }
    else
        cout << "[Successfully attach semaphore to shared memory!]" <<endl;

    int sval;
    if(sem_getvalue(sem, &sval)!=0){
        cout <<"error in sem_getvalue"<<endl;
        exit(1);
    }
    cout << "[Initial semaphore value] is " << sval<< endl;
    glob->value= 1;                                  /* initialize glob */
    cout << "How many times do you want to fork? ";
    cin >> fork_n;
  
    /* Forking processes */
    for (i = 0; i < fork_n; i++){
        pid = fork();
        if (pid < 0) {       /* Error check */    
            cout << "fork error" << endl;
            exit(1);
        }
        else if (pid == 0)  /* Child process */
            break;   
        else{
            pcb* process = new pcb {pid, NULL};
     
            Enqueue(process);
        }               
    }
    
    if (pid > 0){ /* Parent Process (pid > 0) */ 
     
        signal(SIGALRM, &sigalrm_handler);  // set a signal handler
        alarm(2);  // set an alarm for 10 seconds from now
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
            else
                sleep(1);
        }
        /*  shared memory detach 
            http://cgi.di.uoa.gr/~ad/k22/k22-lab-notes4.pdf
        */
        int status= shmdt (glob);
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

        cout << "===================================================" << endl;

        /* cleanup semaphores */
        sem_destroy (sem);
        exit(0);
      
    }
    else{   /* Child process */        
        sem_wait(sem);
        {
            lock_guard<mutex> locker (m);
            cout << "Pid "<< getpid() << " is in the critical section now " << endl;
            sleep(3);
            cout << "Pid "<< getpid() << " is running " << endl;
          
        }
         
        {
            lock_guard<mutex> locker (m_head);
            sem_post(sem);
            if(sem_getvalue(sem, &sval)!=0){
                cout <<"error in sem_getvalue"<<endl;
                exit(1);
            }

            cout << "Process " << getpid() <<" is done. New semaphore value is " << sval<< endl<<endl;
            
            exit(0);
        }
        

    }
}
