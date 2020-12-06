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

	while(cin >> x){
		
      cout << "R: data is " << x <<endl; 
      *ready = 0;
      sleep(4);
      
      *ready = 1;
	}


   cout <<"finished"<<endl;
	
}