/*
    This program uses pipe as a way for inter-process communication
    It's between parent and child process.
    It will prints PING - receive the message and PONG - send the message in a pipe
    With signal added 
*/  
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <cassert>
using namespace std;



int ping[2];        // pipe #1 parent writes to pipe, child reads from pipe
int pong[2];        // pipe #2 child writes to pipe, parent reads from pipe

// http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
void handle_sigchld(int sig) {
    close(ping[0]);
    close(ping[1]);
    close(pong[1]);
    close(pong[0]);
    int saved_errno = errno;
    while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
    errno = saved_errno;
}


int main(){
    pid_t pid;
    int n = 4;          //number of ping-pong 
    int mess = 100;  //the message
    int i;
    int mess_ping;
    int mess_pong;

    /* Explicitly set the SIGCHLD handler to SIG_IGN */
    struct sigaction sa;
    sa.sa_handler = &handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP | SA_NOCLDWAIT;
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
      perror(0);
      exit(1);
    }
    /*********************************************************/
    /* Opening the pipes and error checking */
    if(pipe(ping) == -1 || pipe(pong) == -1){   
        cout <<"Pipe creation failure " << endl;
        exit(1);
    }

    itimerval itimer;
    int fd = 0;
    fd_set writefds, readfds;
    timeval timeout;
    int ret, sret;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    /* write to the pipe */
    assert(write(pong[1], &mess, sizeof(mess))>0);  
   // cout <<"\nFirst time writing to the pipe: "<< mess<< " - Ping " <<endl;
 
    pid = fork();
    for(i =0; i<n; i++){
        if(pid == 0){ // Child process
            close(ping[1]);
            close(pong[0]);
            assert(read(ping[0], &mess_pong, sizeof(mess_pong))>0);   //reading from ping
            cout <<"Child Process : I have received :"<< mess_pong<< " - [PONG] " <<endl;
            assert(write(pong[1], &mess_pong, sizeof(mess_pong))>0);  //writing to pong
       
        }   
        else if(pid>0){  // parent
            FD_ZERO(&readfds);
            FD_SET(pong[0], &readfds);

            //  https://www.youtube.com/watch?v=qyFwGyTYe-M
            /* The select() function indicates which of the specified file descriptors is ready for reading, 
                ready for writing, or has an error condition pending. 
            */
            sret = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout); 
            if(sret == 0){ 
                cout << "sret  = " << sret << " timeout " << endl;

            }
            else if(sret <0){
                cout <<"error"<<endl;
                exit(1);            
            }
            else{
                close(ping[0]);
                close(pong[1]);
                assert(read(pong[0], &mess_ping, sizeof(mess_ping))>0);   //reading from pong
                mess_ping++;
                assert(write(ping[1], &mess_ping, sizeof(mess_ping))>0);  //writing to ping
                cout <<"Parent Process: I have written into the pipe: "<< mess_ping<< " - Ping " <<endl;
            }
        }
        else {
            cout << "fork error" << endl;
            exit(1);
        } 
           
    }
        return 0;
}


