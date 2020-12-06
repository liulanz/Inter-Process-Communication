#include <stdio.h> 
#include <sys/wait.h>  
#include <iostream>
#include <unistd.h>
#include <string>
using namespace std;
#define GRAD_READ pipeML3D[0]
#define GRAD_WRITE pipeML3D[1]
#define D3_READ pipe3DML[0]
#define D3_WRITE pipe3DML[1]

int pipeML3D[2], pipe3DML[2];

/* Program1 sending output to program2 */
void Send(char * program1, char* argv[] ){
    close(GRAD_READ);
    close(D3_READ);
    close(D3_WRITE);
    cout <<"=======================" <<endl;
    cout << "[" << program1 << "] will send data" << endl;
    dup2(GRAD_WRITE , STDOUT_FILENO); /* make output go to pipe */
    execv(program1, argv+1);
    
}

/* Program2 receiving input from program1 */
void Receive(char * program2){
    dup2(GRAD_READ,STDIN_FILENO); /* get input from pipe */
   // dup2(D3_WRITE, STDOUT_FILENO);
    close(GRAD_WRITE);
    close(D3_READ);
    close(D3_WRITE);
    execlp(program2, program2, NULL);
}

int main(int argc, char* argv[]){
    if(argc != 4){
        cout << "invalid number of argument " <<endl;
        return -1;
    }
    int status;     

    /* Creating pipes */
    if(pipe(pipeML3D) == -1 ){  
        perror("Pipe creation failure.");
        exit(EXIT_FAILURE);
    }
    if(pipe(pipe3DML) == -1 ){  
        perror("Pipe creation failure.");
        exit(EXIT_FAILURE);
    }


    int pid = fork();
    if(pid == -1){
        perror("Error creating the pipe");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){ 
        Send(argv[1], argv);
    }
    
    pid = fork();
    if(pid == -1){
        perror("Error creating the pipe");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){ 
        Receive(argv[3]);
    }
    
    close(GRAD_READ);
    close(GRAD_WRITE);
    close(D3_READ);
    close(D3_WRITE);
    waitpid(-1, &status, 0);
    waitpid(-1, &status, 0);
    cout <<"====== END of execution =======" <<endl;
}
   



// pipe between 2 program
  //https://stackoverflow.com/questions/8723628/get-the-output-of-one-c-program-into-a-variable-in-another-c-program

//continuously
//https://stackoverflow.com/questions/40936508/using-pipe-to-pass-data-between-two-processes-continuously


//https://www.unix.com/programming/58138-c-how-use-pipe-fork-stdin-stdout-another-program.html


//pipe
//http://web.stanford.edu/~hhli/CS110Notes/CS110NotesCollection/Topic%202%20Multiprocessing%20(3).html

/*
pipe - Returns a pair of file descriptors, so that what's written to one can be read from the other.

fork - Forks the process to two, both keep running the same code.

dup2 - Renumbers file descriptors. With this, you can take one end of a pipe and turn it into stdin or stdout.

exec - Stop running the current program, start running another, in the same process.
*/