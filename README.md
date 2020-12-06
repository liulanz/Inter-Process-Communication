# Inter-Process-Communication

#### multiprocesses

**binarySemaphore.cpp:** This program uses binary semaphore to regulate the access in critical sections. An simple example that shows how binary semaphore work.

**binarySemaphore_rw.cpp:** This program uses binary semaphore to regulate the access in critical sections. It shows an exmaple of how processes can read at same time and write mutual exclusively

**pipe.cpp:** This program uses pipe as a way for inter-process communication. It's between a parent and a child process. It will prints 'PING' when receiving the message from the pipe and 'PONG' - send the message to a pipe

**timer_interrupt_semaphores.cpp:**  This program has a semaphore value of 3. It uses counting semaphore to let the processes enter the shared resource. It uses semaphore and mutex to protect the shared resource, so only one can modify the shared resource at a time. This program also implements external interrupt with a timer signal. When there's a timer interrupt, the current running process will be pause and move to the back of the queue, and the next child process will start running.

#### multiprocess

**lock_guard.cpp:** This program uses lock_guard to replace m.lock() and m.unlock() lock will be released once it goes out of scope 

**deadlock.cpp:** This program implements deadlock situation as well as the solutions to handle it

**try_catch_exception.cpp:** This program implements exceptions with lock_guard. Throws an exception when it's not even

**scheduling.cpp:** This program shows FCFS schedulings with multiple threads


#### [Inter-Process-Communication](https://github.com/liulanz/Inter-Process-Communication/tree/main/ipc)


### Execute:

Programs in "multiprocesses examples" can only run on UNIX system

```g++ -o filename filename.cpp -pthread```

Programs in "multithreading examples" 

```g++ -o filename filename.cpp ```
	
### Run
``` ./filename```
