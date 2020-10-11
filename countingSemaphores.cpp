// counting semaphores with 1 resource

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
#include <queue>
using namespace std;

                 
// int shmid;                   
// sem_t *sem;                
// pid_t pid;                    
// int *glob;                  
// int fork_n;               
// int sem_num = 4;    
// //int read_count = 0;
// //sem_t *r_mutex;
// int r_num = 2;
// queue<pid_t> glob_q;
//mutex m;

// kill(pid, SIGSTOP) will stop the process, kill(pid, SIGCONT) will continue it

// static void usr1_signal_handler(int signo)
// {
//     cout << "Received PAUSE!" << endl;
// }


/*
    https://www.cs.uic.edu/~jbell/CourseNotes/OperatingSystems/5_Synchronization.html
*/
// void wait(){
//     sem_trywait(sem);
//     sem_num--;
//     //cout <<sem_num <<endl;
//     if(sem_num < 0 ){
//         glob_q.push(getpid());
//         cout << "child process pid "<< getpid() << " waiting on queue "<< endl; 
//         if (kill (getpid(), SIGSTOP) == -1) {
//              perror ("kill of child failed"); exit (-1);
//         }
        
//     }
//     // sem_trywait(sem);

// }
// void signal(){
//     sem_post(sem);
//     sem_num++;
//     if(sem_num <=0){
//         pid_t currentP = glob_q.front();
//         glob_q.pop();
//         // kill (currentP, SIGCONT);
//         if (kill (currentP, SIGCONT) == -1) {
//              perror ("child failed to continue"); exit (-1);
//         }
//     }

// }
// void WriteFunction(int i){
//     sem_wait(r_mutex);
//     sem_num--;

//     kill(getpid(), SIGSTOP);
//     cout << "!!!!Child " << getpid() << " is waiting in queue " << endl;
//     glob_q.push(getpid());
//     kill(glob_q.front(), SIGCONT);
//     cout << "!!!!Child " << getpid() << " is running again " << endl;
//     glob_q.pop();
//     // cout <<i<<endl;
//     // if(!glob_q.empty()){
//     //     
//     // }

//     //sem_trywait(sem);
//   //  cout <<" \t\t<======= Child " << i << " is in critical section [WRITING] =======> " << endl;
//     *glob *= 10 ; 
//    // sem_post(sem);
//     sem_post (r_mutex);
//     sem_num++;
// }
/*
    https://www.cs.uic.edu/~jbell/CourseNotes/OperatingSystems/5_Synchronization.html
*/
// void ReadFunction( int i ){

//     // wait(); 
//     sem_wait(sem);
//     sem_num--;
//     read_count++;
//     if(read_count == 1)   /* First read process will lock */
//         sem_wait (r_mutex); 
//      // signal();  
    
//     sem_post(sem);
//     sem_num++;
//     cout <<" Child " << i << " is in critical section (READING) =======> " << " The value of glob is "<< *glob <<endl;
    
//     // wait(); 
//     sem_wait(sem);
//     sem_num--;
//     read_count--;
//     if(read_count ==0)
//        sem_post (r_mutex); /* Last read process will release the r_mutex lock */
//     //signal();
//     sem_post(sem);
//     sem_num++;
// }



// void countingSemaphores(sem_t * sem, pid_t pid, shared_resource *glob){
//     int sval;
//     sem_wait(sem);
//     // sem_getvalue(sem, &sval);
//     // cout <<sval <<endl;
    
//     // glob->value++;
//     // sleep(2);
//      // m.lock();
  
//     glob->resource_q.push(pid);
//     cout <<" .." <<endl;
//     cout << "PID " << pid<< " is waiting in queue " <<endl;
//     // if(kill(getpid(), SIGSTOP)!=-1)
//     //     cout <<"stopping" <<endl;
//     cout << "PID " << pid<< " is running again " << glob->value++ <<endl;
//      // m.unlock();
//     sem_post(sem);
//     // sem_getvalue(sem, &sval);
//     // cout << sval<< "Child is done." <<endl;
//     // if(!resource_q.empty()){

//     // }
//     // cout << "child pid " << pid << " is currently waiting in queue" <<endl;

//     // glob->resource_q.push(getpid());
//     // if(!glob->resource_q.empty()){
//     //     pid_t front = glob->resource_q.front();
        
//     //     cout << "child pid " << front << " is currently in critical Section " <<endl;
//     //     glob->resource_q.pop();
//     //     //sleep(2);

//     // }
    

// }
struct shared_resource{
    int value;
    queue<pid_t> resource_q;

};
struct pcb{
    pid_t pid;
    pcb* next;
};

pcb * head; 
mutex m;

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
    cout << "<<<<< PID " << p->pid << " is added to queue" <<endl;
    p->next = NULL;
    
    m.unlock();
}

pcb* Dequeue(){
    pcb * front;
    m.lock();
    front = head;

    if(front != NULL){
        head = front->next;

    }
    m.unlock();
    return front;
}

int main ( ){
    sem_t *sem; 
    head = NULL;
    int shmid;                                  
    pid_t pid;                    
    shared_resource *glob;
    int fork_n;               
    int sem_num = 3;    
    //int read_count = 0;
    //sem_t *r_mutex;
    //int r_num = 2;
    //queue<pid_t> glob_q;

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
    sem = sem_open ("write_sem", O_CREAT, 0644, sem_num);
   // sem = (sem_t *)shmat(shmid, NULL, 0);
     sem_init(sem, 1, sem_num);
    // if(sem == (void *) -1){
    //     printf("Error attaching semaphore to shared memory");
    //     exit(1);
    // }
    // else
    //     cout << "[Successfully attach semaphore to shared memory!]" <<endl;

    int sval;
    if(sem_getvalue(sem, &sval)!=0){
        cout <<"error in sem_getvalue"<<endl;
        exit(1);
    }
    cout << "[Initial semaphore value] is " << sval<< endl;
    glob->value= 1;                                  /* initialize glob */
   // cout << "[INITIAL VALUE] glob initial value is " <<  glob->value  << endl;
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
            https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.1.0/com.ibm.zos.v2r1.bpxbd00/rtwaip.htm
        */
        // sleep(2);
        // while(!glob->resource_q.empty()){
        //       pid_t front = glob->resource_q.front();
        //       glob->resource_q.pop();
        //       kill(front, SIGCONT);
        // }
         sleep(10);
         wait(NULL);
        // return 0;
        
        if ((pid = waitpid(pid, &status, WNOHANG)) == -1)
          perror("wait() error");
        else if (pid == 0) {
          time(&t);
          printf("child is still running\n");
          sleep(2);
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
    // sem_destroy (r_mutex);
        exit(0);
      
    }
    else{   /* Child process */
   
       // ReadFunction(i+1);
       // WriteFunction(i+1);
       // ReadFunction(i+1);

       //countingSemaphores(sem, getpid());

   
    // if(sem_getvalue(sem, &sval)!=0){
    //     cout <<"error in sem_getvalue"<<endl;
    //     exit(1);
    // }
    // cout << "[semaphore value] is " << sval<< endl;
    sem_wait(sem);

    m.lock();
    if(sem_getvalue(sem, &sval)!=0){
        cout <<"error in sem_getvalue"<<endl;
        exit(1);
    }
    cout << "[semaphore value] is " << sval<< endl;
    pcb* process = new pcb {getpid(), NULL};
     m.unlock();
    Enqueue(process);
    sleep(1);    
    pcb* current = Dequeue();
    m.lock();
    cout << ">>>>current running  process PID:" << current->pid <<endl;
    m.unlock();
    sem_post(sem);
    if(sem_getvalue(sem, &sval)!=0){
        cout <<"error in sem_getvalue"<<endl;
        exit(1);
    }
    cout << "Process is done. New semaphore value is " << sval<< endl<<endl;
    

 /***************************************************************************************/
     /***************************************************************************************/
    /************THIS IS THE WHERE I TRY TO PUT PID INTO QUEUE*****************************/
    /***************************************************************************************/
     /***************************************************************************************/
    
    // sleep(1);

    
  //  shared_resource r = *glob;
   //(*glob).resource_q.push(getpid());

    //glob->resource_q.push(getpid());

   // queue<pid_t> *q = &(glob->resource_q);
   // q->push(getpid());


    // cout << "PID " << getpid()<< " is waiting in queue " <<endl;
    // if(kill(getpid(), SIGSTOP)!=-1)
    //     cout <<"stopping" <<endl;
    //cout << "PID " << getpid()<< " is running again " << glob->value++ <<endl;
     // m.unlock();
   
    // sem_getvalue(sem, &sval);
    // cout << "PID " << getpid()<< " is done " << "Current semaphore value is " << sval<< endl;
    // sleep(2);


//     sem_wait(sem);
//     sem_num--;

//     glob_q.push(getpid());
//     cout << "!!!!Child " << getpid() << " is waiting in queue " << endl;
//     if(!glob_q.empty()){
//         pid_t current = getpid();
//         pid_t front = glob_q.front();
//         glob_q.pop();
//         kill(front, SIGCONT);
//        // kill(current, SIGSTOP);

    
// //***** http://www.cs.uni.edu/~fienup/cs143f00/course-notes,-in-class-activitie/lecture7.lwp/odyframe.htm
        
//         cout << "current ruuning " << getpid() << "front " << front << "currrent" << current <<endl;
//         //kill(current, SIGSTOP);
//         cout << "!!!!Child " << getpid() << " is running again\n\n " <<endl;
        
  //  }
    
    
    // kill(getpid(), SIGSTOP);
    // sleep(3);

    
    // cout <<i<<endl;
    // if(!glob_q.empty()){
    //     
    // }

    //sem_trywait(sem);
  //  cout <<" \t\t<======= Child " << i << " is in critical section [WRITING] =======> " << endl;
   // *glob *= 10 ; 
   // sem_post(sem);
   // sem_post (sem);

           
       
    }
}