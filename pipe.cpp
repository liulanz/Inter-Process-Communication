  
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>
using namespace std;

int main(){
    int n = 3;          //number of ping-pong 
    int mess = 100;  //the message
    pid_t pid;
    int ping[2];        // pipe #1 parent writes to pipe, child reads from pipe
    int pong[2];        // pipe #2 child writes to pipe, parent reads from pipe
    int mess_ping;
    int mess_pong;
    int stat; 
    int ret;
    /* check check for pipe */
    if(pipe(ping) == -1 || pipe(pong) == -1){   //Opening the pipes and error checking
        cout <<"Pipe creation failure " << endl;
        exit(1);
    }
    //signal (SIGCHLD, proc_exit);
    /* ssize_t write(int fd, const void *buf, size_t count); */
    write(pong[1], &mess, sizeof(mess));    //Sending the message to the parent pipe
   // signal (SIGCHLD, proc_exit);
    pid = fork();
    
    for(int i =0; i<n; i++){
     
        if(pid == 0){    //children process
            
                
                close(ping[1]);
                // timeval timeout = { 0, 1750000 }; 
                // ret = settimer(ITIMER_REAL, &timout, )
                read(ping[0], &mess_pong, sizeof(mess_pong));   //reading in ping
                cout <<"Child Process : I have received "<< mess_pong<< " - [PONG] " <<endl;
                close(pong[0]);
                mess_pong++;
                write(pong[1], &mess_pong, sizeof(mess_pong));  //writing in pong
            
       }
        else if(pid >0) { //parent process
            
                close(pong[1]);
                read(pong[0], &mess_ping, sizeof(mess_ping));   //reading in pong
                cout <<"Parent Process : I have received "<< mess_ping<< " - PING " <<endl;
                close(ping[0]);
                write(ping[1], &mess_ping, sizeof(mess_ping));  //writing in pong
            
        }
        else 
            exit(1);
        
    }
   
    // if(pid ==0){
    //     close(ping[0]);
    //     close(pong[1]);
    // }
    // else{
    //     close(ping[1]);
    //     close(pong[0]);
    // }

     if(pid >0){
      
       // time_t t;
        /* 
            wait for child processes to end 
            https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.1.0/com.ibm.zos.v2r1.bpxbd00/rtwaip.html
            avoiding zombie processes is to call waitpid() at least once for each child. Invoking waitpid() with -1 
            as the pid argument will check on an exit status of an arbitrary child.
        */
        do{
            if ((pid = waitpid(pid, &stat, WNOHANG)) == -1)
               perror("wait() error");  /* print a system error message */
            else if (pid == 0) { 
                // sleep(1);
                // time(&t);
                // printf("child is still running at %s", ctime(&t));
                // sleep(1);
            }
            else {
              if (WIFEXITED(stat))
                printf("child exited with status of %d\n", WEXITSTATUS(stat));
              else puts("child did not exit successfully");
            }
        }while (pid == 0);
     }

    return 0;
}