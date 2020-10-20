/* 
    This program creates a FCFS scheduling with threads
*/
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <pthread.h> 
#include <iostream>
using namespace std;


#define NUM_THREADS 5

struct pcb_t{
    pthread_t thread_pid;
    struct pcb_t * next;
};


// Declaration of thread condition variable 
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER; 
// declaring mutex 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

int done = 1; 
int glob = 0; 
// queue<pthread_t> ready_Q;
pcb_t *head;

pthread_mutex_t m;
  
void add_to_queue(pcb_t * pcb){
    pcb_t * curr;
    pthread_mutex_lock(&m);
    curr = head;

    if(curr == NULL){
        head = pcb;
        
    }
    else
    {
        while(curr->next!=NULL){
            curr= curr->next;
        }
        curr->next = pcb;
    }
    pcb->next = NULL;
    
    pthread_mutex_unlock(&m);
}
pcb_t* pop_fron_queue(){
    pcb_t * front;
    pthread_mutex_lock(&m);
    front = head;

    if(front != NULL){
        head = front->next;

    }
    pthread_mutex_unlock(&m);
    return front;
}

void* ThreadFunction(void *vargp) 

{ 

    pthread_mutex_lock(&m);
    cout << "Thread ID: " << pthread_self() << " is here" << endl;
    
    pcb_t * thread = new pcb_t {pthread_self(), NULL};
    pthread_mutex_unlock(&m);
 
   
    add_to_queue(thread);

    sleep(3);
    
    pcb_t * running_thread = pop_fron_queue();

    pthread_mutex_lock(&m);
    cout << "Thread ID: " <<  running_thread-> thread_pid<< ", Global: "<< ++glob <<endl;
    pthread_mutex_unlock(&m);
    return NULL;
} 
  
int main() 
{ 
    int i, res; 
    pthread_t threads[NUM_THREADS];
    head = NULL;
    pthread_mutex_init(&m, NULL);

    for( i=0; i < NUM_THREADS;i++ )    
    {

    /*int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start)(void *), void *arg);
      pthread_create(&tid, NULL, myThreadFun, (void *)&tid); 
       pthread_create(&tid, NULL, ThreadFunction, NULL); 
    */
      // returns zero when the call completes successfully. Any other return value indicates that an error occurred. 
      res = pthread_create(&threads[i], NULL, ThreadFunction,  NULL);

      if (res!=0){
         cout << "Error:unable to create thread." << endl;
         exit(1);    
      }    
    }   
  
    pthread_exit(NULL); 

    return 0; 
} 