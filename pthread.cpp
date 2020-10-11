/** https://github.com/dkuo7/CS2200/blob/master/Project6/student.c*/
#include<iostream>    
#include<cstdlib>    
#include<pthread.h>

using namespace std;

#define NUM_THREADS 5

struct thread_data
{
  int  thread_id;
  char *message;
};


void *PrintHello(void *threadarg)
{
   struct thread_data *my_data;   

   my_data = (struct thread_data *) threadarg;

   cout << "Thread ID : " << my_data->thread_id ;

   cout << " Message : " << my_data->message << endl;

   /* terminate calling thread*/
   pthread_exit(NULL);
}

int main ()
{
   pthread_t threads[NUM_THREADS];

   struct thread_data td[NUM_THREADS];

   int rc, i;


   for( i=0; i < NUM_THREADS; i++ )    
   {

      cout <<"main() : creating thread, " << i << endl;

      td[i].thread_id = i;

      td[i].message = "This is message";


/*int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start)(void *), void *arg);
  pthread_create(&tid, NULL, myThreadFun, (void *)&tid); 
*/
      rc = pthread_create(&threads[i], NULL, PrintHello, (void *)&td[i]);

      if (rc){

         cout << "Error:unable to create thread," << rc << endl;

         exit(-1);    
      }    
   }   
   
   /* terminate calling thread*/
   pthread_exit(NULL);    
}