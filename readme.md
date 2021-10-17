# COL 331 : OPERATING SYSTEM
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
