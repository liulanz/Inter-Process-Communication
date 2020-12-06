#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
using namespace std;
int main() {
   key_t key = ftok("shmfile",65); 
   int shmid = shmget(key, sizeof (int),0666|IPC_CREAT); // shmget returns an ide in shmid
   int *ready = (int*) shmat(shmid,(void*)0,0); // shmat to join to shared memory

   string x;
   int count = 0;
  *ready = 1;
	while(cin >> x){
		
      cout << "Reading: data is " << x <<endl; 
      count++;
      if(count %5 ==0){
        *ready = 0;
        sleep(1);
      }
      *ready = 1;
	}


   cout <<"finished"<<endl;
	
}