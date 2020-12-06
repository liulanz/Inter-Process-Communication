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
#include <signal.h>
using namespace std;

#define GRAD_READ pipeML3D[0]
#define GRAD_WRITE pipeML3D[1]
#define D3_READ pipe3DML[0]
#define D3_WRITE pipe3DML[1]

struct pcb{
    pid_t pid;
    pcb* next;
};

int main(int argc, char* argv[]){
    if(argc != 3){
        cout << argc <<endl;
        cout << "invalid number of argument " <<endl;
        return -1;
    }
    int* pids;
    int pipeML3D[2], pipe3DML[2];
    int *ready;
    int shmid1, shmid2;  
    int i;
    int status;  
    pcb * head;   
    pid_t pid;
    
    key_t key = ftok("shmfile",65); 
    shmid1 = shmget (key, sizeof (int), IPC_CREAT | 0666);
    shmid2 = shmget (IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
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
  
    *ready = 1;
    pids = (int *) shmat (shmid2, NULL, 0);   
    if(*pids == -1){
        cout << "Attachment errr" << endl;
        exit(1);
    }

    if(pipe(pipeML3D) == -1 ){  
        perror("Pipe creation failure.");
        exit(EXIT_FAILURE);
    }
    if(pipe(pipe3DML) == -1 ){  
        perror("Pipe creation failure.");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < 2; i++){
        pid = fork();
        if (pid < 0) {       /* Error check */    
            cout << "fork error" << endl;
            exit(1);
        }
        else if (pid == 0)  /* Child process */
            break;   
        else{
            if(i ==0){
                pids[0] = (int)pid;
               
            }
            else if(i==1){
                pids[1] = (int)pid;
    
            }
        }
                
    }
    if (pid > 0){ /* Parent Process */ 
        int endID = waitpid(pids[0], &status, WNOHANG|WUNTRACED);
        while (endID!=pids[0]){ /*while write-program has not yet terminated */
        //while(*ready !=2){
            /* when read-program sets ready to 0 meaning pause on sending data */
            if(*ready == 0){ 
               // cout << "pausing" <<endl;
                kill((pid_t)pids[0], SIGSTOP);
            }
            /* when read-program sets ready to 0 meaning pause on sending data */
            else if(*ready ==1){ 
                cout << "resuming" <<endl;
                kill((pid_t)pids[0], SIGCONT);
            }
           // endID = waitpid(pids[0], &status, WNOHANG|WUNTRACED);
        }
        
        *ready = 2; /* ready = 2 stating the write-program finished sending all data */
       endID = waitpid(pids[1], &status, WNOHANG|WUNTRACED);
        /* closing the pipe */
        if(endID != pids[1]){ 
            close(GRAD_READ);
            close(GRAD_WRITE);
            close(D3_READ);
            close(D3_WRITE);
        }
        /* wait until read finishes reading the rest of the data */
        while(status = (int)waitpid(pids[1], &status, WNOHANG|WUNTRACED)!= pids[1] ){
            if(status==-1)
                break;
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
        status = shmdt (pids);
        if(status == -1){
            cout << "Error: Remove shared memory segment  " << endl;
            exit(1);
        }
        status = shmctl (shmid2, IPC_RMID, 0); 
        if(status == -1){
            cout << "Error: Remove shared memory segment  " << endl;
            exit(1);
        }
        cout << "done"<<endl;
        exit(0);
      
    }
    else{   /* Child process */  
        if(getpid() == (pid_t)pids[0]){ //running send process 
            close(D3_READ);
            close(D3_WRITE);
            dup2(GRAD_WRITE , STDOUT_FILENO); /* make output go to pipe */
            execlp(argv[1], argv[1], NULL);
            // char* arr[] = {"./svm.out", "data_monks/monks-1.train", "data_monks/monks-1.test", "400000",".001", NULL};
            // execv(argv[1], arr);
        }
        else{ // runing receive process
            dup2(GRAD_READ,STDIN_FILENO); /* get input from pipe */
            close(GRAD_WRITE);
            close(D3_READ);
            close(D3_WRITE);
            execlp(argv[2],argv[2], NULL);
        }
        
   }
        
}
   

