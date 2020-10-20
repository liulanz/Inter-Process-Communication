/*
    This program uses binary semaphore to regulate the access in critical sections

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
using namespace std;

                       
int shmid;                   
sem_t *sem;                
pid_t pid;                    
int *glob;                  
int fork_n;               
int sem_num = 2;    /* binary semaphore */
int read_count = 0;
sem_t *r_mutex;
int r_num = 2;

/*
    https://www.cs.uic.edu/~jbell/CourseNotes/OperatingSystems/5_Synchronization.html
*/
void WriteFunction(int i){
    sem_trywait(r_mutex);
    //sem_trywait(sem);
    cout <<" \t\t<======= Child " << i << " is in critical section [WRITING] =======> " << endl;
    *glob *= 10 ; 
   // sem_post(sem);
    sem_post (r_mutex);
}
/*
    https://www.cs.uic.edu/~jbell/CourseNotes/OperatingSystems/5_Synchronization.html
*/
void ReadFunction( int i ){

    sem_trywait (sem); 
    read_count++;
    if(read_count == 1)   /* First read process will lock */
        sem_wait (r_mutex); 
    sem_post (sem);  
    cout <<" Child " << i << " is in critical section (READING) =======> " << " The value of glob is "<< *glob <<endl;
    sem_trywait (sem); 
    read_count--;
    if(read_count ==0)
       sem_post (r_mutex); /* Last read process will release the r_mutex lock */
    sem_post (sem);
}
int main ( ){
    int i;
    time_t t;
    int status;

    cout << "===================================================" << endl;
    /*  
        Creating shared memory for int, 
        IPC_PRIVATE: a uniquely generated key 
        http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/shm/shmget.html
    */
    shmid = shmget (IPC_PRIVATE, sizeof (int), IPC_CREAT | 0666);
    if (shmid < 0) {                           /* shmget error check */
        cout << "shmget error" << endl;
        exit (1);
    }
    
    /*
        Attach shared variable to shared memory using shmat 
        http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/shm/shmat.html
    */
    glob = (int *) shmat (shmid, NULL, 0);   
    if(*glob == -1){
        cout << "Attachment errr" << endl;
        exit(1);
    }
    else
        cout << "\n[Successfully attached shared variable to shared memory]" << endl;
    *glob = 1;                                  /* initialize glob */
    cout << "\n[INITIAL VALUE] glob initial value is " << *glob  << endl;

    cout << "How many times do you want to fork? ";
    cin >> fork_n;


    /* 
        initialize semaphores for shared processes 
        sem_t *sem = sem_open(SNAME, O_CREAT, 0644, 3);
        sem_t *sem_open(const char, *name, int oflag, mode_t mode, unsigned int value);
    */
    sem = sem_open ("write_sem", O_CREAT, 0644, sem_num);
    r_mutex = sem_open ("read_sem", O_CREAT, 0644, r_num);
  
    /* Forking processes */
    for (i = 0; i < fork_n; i++){
        pid = fork();
        if (pid < 0) {       /* Error check */    
            cout << "fork error" << endl;
            exit(1);
        }
        else if (pid == 0)  /* Child process */
            break;                  
    }

    if (pid > 0){ /* Parent Process (pid > 0) */ 

        while ((pid = wait(&status)) > 0); 
        cout << "\n[Parent Process: All children have exited]." <<endl;

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
        sem_destroy (r_mutex);
        exit (0);
    }
    else{   /* Child process */

        ReadFunction(i+1);
        WriteFunction(i+1);
        ReadFunction(i+1);
           
        exit (0);
    }
}