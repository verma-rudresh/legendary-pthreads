#include "../include/myThread.h"
#include "../include/myArray.h"
#include <stdlib.h> 
#include <setjmp.h> 
#include <stdio.h>
#include <string.h> 
#include <time.h>
#include <signal.h> 
#include <errno.h>

#define INTERVAL 50
#define STACKSIZE 40960
#define JB_SP 6
#define JB_PC 7

int cur_tID = 0;

myThread_tcb* master;   // main thread
myThread_tcb* current; // current running threads
myArray_t* thread_list; // threads which are ready to run next


/*Timer starting*/
struct itimerval timer;
struct sigaction sa;

void start_time() {
	static sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void stop_time() {
	static sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

static long int manglex64(long int p) {
    long int ret;
    asm(" mov %1, %%rax;\n"
        " xor %%fs:0x30, %%rax;"
        " rol $0x11, %%rax;"
        " mov %%rax, %0;"
        : "=r"(ret)
        : "r"(p)
        : "%rax");
    return ret;
}


void timer_handler(int signum) {
    stop_time();

    if(current->status == RUNNING) {
        current->status = READY;
    }
    if (setjmp(current->buf)) {
        current->status = RUNNING;
        return;
    } 
    else {
        myThread_tcb* next;
        while(1) {
            next = scheduler_choose_task();
            if(next->status == READY) break;
        }
        current = next;
        start_time();
        longjmp(current->buf, 1);
    }
}

void myThread_init() {
    cur_tID = 0;
    current = NULL;
    thread_list = (myArray_t*)malloc(sizeof(myArray_t));
    current = (myThread_tcb*)malloc(sizeof(myThread_tcb));
    init_array(thread_list); 
 
    // setting the main thread
    master = (myThread_tcb*)malloc(sizeof(myThread_tcb));
    master->tid = 0;
    master->status = RUNNING;
    master->func = NULL;
    master->arg = NULL;
    master->stackPointer = NULL;
    master->blocked_join_on_tid = -1;
    current = master;
    push_array(thread_list, master);

    memset(&sa, 0, sizeof sa);
    sa.sa_handler = timer_handler;
    sa.sa_flags = SA_NODEFER;
    sigaction (SIGALRM, &sa, 0);

    timer.it_value.tv_sec= INTERVAL/1000;
    timer.it_value.tv_usec= (INTERVAL*1000) % 1000000;
    timer.it_interval = timer.it_value;
    start_time();
}


myThread_tcb* scheduler_choose_task(void) {
    /*
    * this is a FIFO implementation, or simply round robin fashion
    */
    myThread_tcb* task = pop_array(thread_list);
    push_array(thread_list, task);
    return task;
}


void freeThreads(void)
{
    stop_time();
    while(size_array(thread_list) > 0){
        remove_ele(thread_list,peak_front(thread_list)->tid);
    }
    free(thread_list);
    thread_list = NULL;
    cur_tID = 0;
}


void wrapper() {
    start_time();
    current->func(current->arg);
    //put exit thread here
    myThread_exit();
}

int myThread_create(myThread_t *thread, void *(*function)(void *), void *arg) {
    stop_time();
    if(cur_tID==0) {
        myThread_init();
    }
    myThread_tcb* newThread;
    newThread = (myThread_tcb*)malloc(sizeof(myThread_tcb));
    newThread->tid = ++cur_tID; //tID starts from 1
    *thread = cur_tID;
    newThread->status = READY;
    newThread->func = function;
    newThread->arg = arg;
    newThread->blocked_join_on_tid = -1;
    newThread->stackSize = STACKSIZE;
    newThread->stackPointer = (unsigned long *)malloc(STACKSIZE);
    push_array(thread_list, newThread);

    if(setjmp(newThread->buf) == 0)
    {
        newThread->buf[0].__jmpbuf[JB_SP] = manglex64((unsigned long)(newThread->stackPointer + (STACKSIZE-8) / 8 - 2));
        newThread->buf[0].__jmpbuf[JB_PC] = manglex64((unsigned long) wrapper);
    }

    start_time();
    return 0;
}


/* 
Give up the CPU and allow the next thread to run.
 */
void myThread_yield(){
    stop_time();
    current->status = READY;
    start_time();
    timer_handler(1);
}


int myThread_equal(myThread_t t1, myThread_t t2) {
    // return 1 if both threads have same id
    return t1 == t2;
}

/* The calling thread will not continue until the thread with tid thread
 * has finished executing.
 */
int myThread_join(myThread_t thread){
    stop_time();

    if(myThread_equal(thread, myThread_self())) {
        // waiting on itself is not a successful operation
        return 1;
    }

    myThread_tcb* threadToJoin =search_ele(thread_list, thread);
    if(!threadToJoin){
        printf("Tried to join non-existing thread");
        exit(0);
    }

    threadToJoin->blocked_join_on_tid = current->tid;

    if(threadToJoin->status == FINISHED){
        // the thread is no longer active, so simply return
        start_time();
        return 0;
    }
    else{
        current->status = BLOCKED_JOIN;
        start_time();
        // pause so that scheduling can be done
        timer_handler(1);
    }
    return 0;
}


myThread_t myThread_self(){
    return (current->tid);
}


void myThread_exit(void)
{
    stop_time();
    current->status = FINISHED;
    int parentID = current->blocked_join_on_tid;
    if(parentID != -1) {
        myThread_tcb* parentThread = search_ele(thread_list, parentID);
        if(parentThread) parentThread->status = READY;
    }
    start_time();
    timer_handler(1);
}

int myThread_cancel(myThread_t thread) {
    stop_time();
    if (myThread_equal(thread, myThread_self())) {
        /* If it is the current thread, reschedule. */
        start_time();
        timer_handler(1);
        return 1;
    }

    myThread_tcb* task = search_ele(thread_list, thread);
    if(!task) return 1;

    current->status = CANCELLED;
    int parentID = current->blocked_join_on_tid;
    if(parentID != -1) {
        myThread_tcb* parentThread = search_ele(thread_list, parentID);
        if(parentThread) parentThread->status = READY;
    }
    start_time();
    timer_handler(1);
    return 0;
}


/*----------------------- MUTEX LOCK --------------------------*/
int myThread_mutex_init (myThread_mutex_t *mutex) {
	mutex->lock = 0; /*Initially it is unlocked*/
	return 0;
}

int myThread_mutex_lock (myThread_mutex_t *mutex) {
    stop_time();
	while (mutex->lock == 1) {
        current->status = BLOCKED_SEMAPHORE;
        start_time();
        timer_handler(1);
    }
    mutex->lock = 1;
    start_time();
    return 1;
}

int myThread_mutex_unlock(myThread_mutex_t *mutex) {
    stop_time();
	mutex->lock = 0;
    for(int i=1; i<=cur_tID; i++) {
        myThread_tcb* th = search_ele(thread_list, i);
        if(th->status == BLOCKED_SEMAPHORE) {
            th->status = READY;
        }
    }
    start_time();
    timer_handler(1);
	return 1;
}

