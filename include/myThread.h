#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <setjmp.h>
#include "myArray.h"

/* Function Declarations */
int myThread_create(myThread_t *thread, void* (*function)(void*), void *arg);
myThread_t myThread_self();
void myThread_yield(void);
int myThread_cancel(myThread_t thread);
void myThread_exit(void);
int myThread_join(myThread_t thread);
myThread_tcb* scheduler_choose_task(void);
void schedule(void);
void timer_handler(int signum);
void myThread_init(void);
void freeThreads(void);


/*---------- MUTEX LOCKS ----------*/
typedef struct {
	int lock; /*stores 1 if locked else 0*/
} myThread_mutex_t;

int myThread_mutex_init(myThread_mutex_t *mutex);
int myThread_mutex_lock(myThread_mutex_t *mutex);
int myThread_mutex_unlock(myThread_mutex_t *mutex);

#endif 