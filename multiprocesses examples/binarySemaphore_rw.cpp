/*
    This program uses binary semaphore to regulate the access in critical sections
    It allows read at same time and write mutual exclusively

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
using namespace std;

                       
int shmid;                   
sem_t *sem;                
pid_t pid;                    
int *glob;                  
int fork_n;               
int sem_num = 1;  
int read_count = 0;
sem_t *r_mutex;
int r_num = 2;
mutex m;

    
/*
    https://www.cs.uic.edu/~jbell/CourseNotes/OperatingSystems/5_Synchronization.html
*/
void WriteFunction(){
   
    sem_wait(r_mutex);
    sem_wait(sem);
    int sval;
    if(sem_getvalue(sem, &sval)!=0){
        cout <<"error in sem_getvalue"<<endl;
        exit(1);
    }

   cout <<"semaphore value: " << sval <<" child pid: " <<getpid() << "  is using the shared memory"<<endl;
 
    m.lock();
 //   sleep(1);
    cout <<" <======= Child " << getpid()<< " is in critical section [WRITING] =======> " << endl;
    *glob *= 10 ; 
    m.unlock();
   
    
    sem_post (r_mutex);
    sem_post (sem);
    int s;
    if(sem_getvalue(sem, &s)!=0){
        cout <<"error in sem_getvalue"<<endl;
        exit(1);
    }

    cout <<"semaphore value: " << s <<" child pid: " <<getpid() << "  is done."<<endl;

}
/*
    https://www.cs.uic.edu/~jbell/CourseNotes/OperatingSystems/5_Synchronization.html
*/
void ReadFunction( ){
    int sval;
    sem_wait (sem); 
    m.lock();
    read_count++;
    if(read_count == 1)   /* First read process will lock */
        sem_wait (r_mutex); 

    sem_post (sem); 
     m.unlock(); 

    
    sem_wait (sem); 
    m.lock();
    
    if(sem_getvalue(sem, &sval)!=0){
        cout <<"error in sem_getvalue"<<endl;
        exit(1);
    }
    cout <<"semaphore value: " << sval <<" child pid: " <<getpid() << "  is using the shared memory. The value of glob is "<< *glob <<endl;
    read_count--;
    if(read_count ==0)
       sem_post (r_mutex); /* Last read process will release the r_mutex lock */
    m.unlock();
    sem_post (sem);
     if(sem_getvalue(sem, &sval)!=0){
        cout <<"error in sem_getvalue"<<endl;
        exit(1);
    }

    cout <<"semaphore value: [" << sval <<"] child pid: " <<getpid() << "  is done."<<endl;
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

    


    /* 
        initialize semaphores for shared processes 
        sem_t *sem = sem_open(SNAME, O_CREAT, 0644, 3);
        sem_t *sem_open(const char, *name, int oflag, mode_t mode, unsigned int value);
    */
    // sem_init(sem, 1, sem_num);
   // sem = (sem_t *)shmat(shmid, (void*)0, 0);
    sem = sem_open ("write_sem", O_CREAT, 0644, sem_num);
    sem_init(sem, 1, sem_num);
    r_mutex = sem_open ("read_sem", O_CREAT, 0644, r_num);
     int sval;
    if(sem_getvalue(sem, &sval)!=0){
        cout <<"error in sem_getvalue"<<endl;
        exit(1);
    }
    cout << "[Initial semaphore value] is " << sval<< endl;
    cout << endl;

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
    }

    if (pid > 0){ /* Parent Process (pid > 0) */ 
         /*
            wait for all children to exit 
            pid_t waitpid(pid_t pid, int *status, int options)
            // pid_t waitpid(pid_t pid, int *status, int options);
            // while (pid = waitpid (-1, NULL, 0)){
            //     if (errno == ECHILD)
            //         break;
            // }
            https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.1.0/com.ibm.zos.v2r1.bpxbd00/rtwaip.htm
        */
        sleep(6);
        if ((pid = waitpid(pid, &status, WNOHANG)) == -1)
          perror("wait() error");
        else if (pid == 0) {
          time(&t);
          printf("child is still running\n");
          sleep(1);
        }
        else {
          if (WIFEXITED(status))
            printf("child exited with status of %d\n", WEXITSTATUS(status));
          else puts("child did not exit successfully");
        }
        
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
        
        ReadFunction();
        WriteFunction();
        ReadFunction();
           
        exit (0);
    }
}