# OPERATING SYSTEMS
## My own Thread library similar to POSIX
## Problem Statement:
A quick google tells us that “A thread is a basic unit of CPU utilization, consisting of a program counter, a stack, and a set of registers, (and a thread ID)”.

![image](https://user-images.githubusercontent.com/61209181/137616245-d44cac7b-94e2-4465-a8c1-f7b13bd5d047.png)

User threads, are above the kernel and without kernel support. These are the threads that application programmers use in their programs.
In this assignment, you will create a user level thread level library which implies that it is supported above the kernel in user space and are managed without kernel support.

**Do NOT use the PTHREADS library in any way in this assignment except to compare performance.** 

## Tests:
You will test your implementations using the following application programs:

Multi threaded Matrix Multiplication with N threads 
* Plot a graph with respect to time taken versus N (=1 to 40) 
* Calculate the average context switch overhead with our thread library 
* Repeat the experiment with pThreads and compare the average context switch  overheads inboth cases. 

Bounded Buffer (Producer Consumer) with N containers, M producers and M Consumers (M<N) 
* In this simulation, you should make each producer and consumer print when it acquires a container or deposits a container. 
* You must also maintain a shared buffer that tracks and prints how many containers  have been produced and consumed. The producers and consumers should each stop after 10 X N items have been either produced or consumed. 

Your thread library needs to be built into a dynamically-linked shared library.  

## Features:
* Creating, joining, yielding, exiting, canceling, self and FIFO scheduling of threads.
* Mutex_locks to ensure synchronization.

## Assumptions:
* The id of each thread passed to <code>createThread()</code> function must be > 0 and unique.
* After exiting, threads are send to <code>FINISHED</code> state and are later freed via calling <code>freeThreads()</code> function.
* Each thread is assigned its private stack as the jumping of environment through <code>setjmp</code> and <code>longjmp</code> functions. 
* Each thread must have its own stack so that returning from the function don’t mess-up the stack and we don’t loose the progression of the function the thread is running.


## How to run:
* <code>make</code> : compiles and creates dynamically linked library
* <code>make matrix_mult</code> : run the matrix multiplication
* <code>make boundedBuffer N=n M=m</code> : run the producer consumer problem, where n is the number of containers and m is no of producers and consumers both.
* <code>make clean</code> : to clean all the files we build through make.

## Results and Observations:
* In matrix-multiplication, the context switching time of our library was more than that of pthreads because pthreads also perform some optimizations.
* Also, our implementation is not so advanced as compared to pthreads.
* In matrix-multiplication, on increasing the number of threads, the total execution time increased. This is probably as our threads are user-level and hence don’t run concurrently, but run in round-robin fashion. 
* More no of threads increases overheads in thread switching.
* Lock is tested for the producer consumer problem and is working correctly.

## Output:
* <code>matResult.txt</code> stores the information about the context switching time for both our implementation and pthread library.
* <code>ProdCons.txt</code> stores the information about the buying and selling of different items.

* ![image](https://user-images.githubusercontent.com/61209181/137616790-dc11f166-fb93-411f-8eea-52436cb1074e.png)

**Fig.  Graph of Total thread execution time versus number of threads in myOwnThread library.**
