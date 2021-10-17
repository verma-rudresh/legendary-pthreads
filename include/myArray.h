#ifndef MYARRAY_H
#define MYARRAY_H

#include<stdbool.h>

/* Thread ID */
typedef uint myThread_t;

/* Thread Status Enum */
typedef enum threadStatus {
    READY,     
    RUNNING,   
	FINISHED,   
    BLOCKED_JOIN,
    BLOCKED_SEMAPHORE,
    CANCELLED
} status_t;

/*  Thread Contol Block */
typedef struct threadControlBlock{
  myThread_t tid;   
  status_t status;   
  jmp_buf buf;
  unsigned long *stackPointer;
  int stackSize;
  void* (*func)(void*);
  void *arg;
  void *retVal;
  myThread_t blocked_join_on_tid;
} myThread_tcb;

/* Queue to hold all the threads created */
typedef struct myArray_t 
{
    myThread_tcb* array;
    int size;
} myArray_t;

int size_array(myArray_t* arr);
void init_array(myArray_t* arr);
void push_array(myArray_t* arr, myThread_tcb* item);
myThread_tcb* pop_array(myArray_t* arr);
bool isempty_array(myArray_t* arr);
myThread_tcb* search_ele(myArray_t* arr, int id);
myThread_tcb* remove_ele(myArray_t* arr, int id);
void print_array(myArray_t* arr);
myThread_tcb* peak_front(myArray_t* arr);

#endif 