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


struct pcb{
    pid_t pid;
    pcb* next;
};

int main(int argc, char* argv[]){
    if(argc != 2){
        cout << argc <<endl;
        cout << "invalid number of argument " <<endl;
        return -1;
    }
    int* pids;
    int pipeML3D[2];
    int *ready;
    int shmid1, shmid2;  
    int i;
    pid_t receive_pid;
    int status;  
    pcb * head;   
    pid_t pid;
    
    key_t key = ftok("shmfile",65); 
    shmid1 = shmget (key, sizeof (int), IPC_CREAT | 0666);
 
    if (shmid1 < 0) {                           /* shmget error check */
        cout << "shmget error" << endl;
        exit (1);
    }
    
    /* Attach shared variable to shared memory using shmat  */
    ready = (int *) shmat (shmid1, NULL, 0);   
    if(*ready == -1){
        cout << "Attachment errr" << endl;
        exit(1);
    }
    *ready = 0;

    
    if(pipe(pipeML3D) == -1 ){  
        perror("Pipe creation failure.");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {       /* Error check */    
        cout << "fork error" << endl;
        exit(1);
    }
    else{
       receive_pid = pid;
    }
            
    
    if (pid > 0){ /* Parent Process */
        dup2(GRAD_WRITE , STDOUT_FILENO); /* make output go to pipe */
        close(GRAD_READ);
        int count = 0;
        while (count < 100) {
            if(*ready ==1){
               cout << count <<endl;
                count++;
               *ready = 0;
            }
        }
        close(GRAD_WRITE);
        close(GRAD_READ);
     
        /* wait until read finishes reading the rest of the data */
        while((status = waitpid(receive_pid, &status, WNOHANG|WUNTRACED))) {
            if(status == receive_pid )
                break;
            else if(status==-1)
                return -1;
            else if(status == 0){
                sleep(1);
            }
        }

        /* deatach shared memory */
        status = shmdt (ready);
        if(status == -1){
            cout << "Error: Remove shared memory segment  " << endl;
            exit(1);
        }   
        status = shmctl (shmid1, IPC_RMID, 0); 
        if(status == -1){
            cout << "Error: Remove shared memory segment  " << endl;
            exit(1);
        }
        exit(0);
      
    }
    else{   /* Child process */  
      
        dup2(GRAD_READ,STDIN_FILENO); /* get input from pipe */
        close(GRAD_WRITE);
        execlp(argv[1],argv[1], NULL);
        
        
   }
        
}
   

