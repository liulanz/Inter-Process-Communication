# INTER-PROCESS-COMMUNICATION

ipc program will pass the output of one executable file to another executable file through pipe. The program redirects the standard input and standard output using ``dup2()``. It also uses ``fork()`` and ``exec()`` to execute the other prcesses.

## ipc.cpp
``ipc.cpp`` takes in 2 arguments: 
1. an executable file that will send data through pipe, e.g. ``./w1.out``
1. an executable file that will receive data from pipe, e.g. ``./rscode``
### run
```
make ipc1
make test1
```
### Expected output
```
=======================
[./w1.out] will send data
This is rust code. I received:
program 1 data<RUST>
====== END of execution =======
```

**P.S. I added ``<RUST>`` in the back of line of messgae to check if it's printing from ``./rscode``**

## ipc2.cpp
``ipc.cpp`` takes in 3 arguments: 
1. an executable file that will send data through pipe, e.g. ``./write.out``
1. an agument that is needed for sending program, e.g. ``4``
1. an executable file that will receive data from pipe, e.g. ``./rscode``
### run
```
make ipc2
make test2
```
### Expected output
```
=======================
[./write.out] will send data
This is rust code. I received:
hello world<RUST>
hello world<RUST>
hello world<RUST>
hello world<RUST>
====== END of execution =======
```

## ipc3.cpp
This program forks 4 child processes. Each will run ``./w1.out``, ``./w2.out``, ``./w3.out``, and ``./rscode`` respectively. ``./w1.out``, ``./w2.out`` and ``./w3.out`` will all write to the pipe, and ``./rscde`` will print all the message from pipe.

This program implements process queue using shared memory and semaphore.
### run
```
make ipc3
make test3
```
### Expected output
```
[Successfully attached shared variable to shared memory]
[./w1.out] will send data
[./w2.out] will send data
[./w3.out] will send data
[./rscode] will receive data
This is rust code. I received:
program 1 data<RUST>
program 2 data<RUST>
program 3 data<RUST>
[Successfully detach shared memory segment].
[Successfully removed shared memory segment].

```

## two_processes_pipe/sendcount_receive.cpp
To avoid data exceeding the capacity of pipe buffer, this program allows the read_program telling the write_program when to send data. There's a integer value stores in the shared memory called ``ready``. Both the write_program and the read_program need to be attached to the shared memory. The write_program can only send data when ``ready = 1`` and keep waiting when ``ready = 0``. When write_program finishes sending all the data, it will close the pipes, so the read_program will not wait for input indefinitely. ``sendcount_receive.cpp`` acts like a write_program, which sends data to ``receive.cpp`` when ``ready == 1``. ``receive.cpp`` uses ``sleep()`` to simulate the time a program processing the data.

### run
```
cd two_processes_pipe/
make
make test
```

## three_processes_pipe_sigstop/pipe_sigstop.cpp
This program forks two processes: one will use ``exec()`` to run the write_program, the other one will use ``exec()`` to run the read_program. There's a integer value stores in the shared memory called ``ready``. The read_program needs to be attached to the shared memory. It will change the value of ready to 0 and 1. The parent will check for the value of ready. If ``ready == 1``, parent would use ``SIGCONT`` to resume the write_program. If ``ready == 0``, parent would use ``SIGSTOP`` to pause the write_program. When write_program terminates, the parent process will close the pipes, so the read_program will not wait for input indefinitely.

### run
```
cd hree_processes_pipe_sigstop/
make
make test
```

## Clean
```
make clean
```