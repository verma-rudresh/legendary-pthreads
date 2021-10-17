# include<stdio.h>
# include<stdlib.h>
# include<stdbool.h>
#include <assert.h>
#include <malloc.h>
#include <setjmp.h>

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
  void (*func)(void*);
  void *arg;
  void *retVal;
  int blocked_join_on_tid;
} myThread_tcb;


typedef struct myArray_t 
{
    myThread_tcb** array;
    int size;
} myArray_t;


void init_array(myArray_t* arr) {
    arr->size = 0;
    arr->array = (myThread_tcb**)malloc(100*sizeof(myThread_tcb*));
    
}

int size_array(myArray_t* arr){
    return arr->size;
}

void push_array(myArray_t* arr, myThread_tcb* item) {
    int y = size_array(arr);
    arr->array[y] = item;
    arr->size++;
}

myThread_tcb* pop_array(myArray_t* arr){
    if(arr->size == 0) return NULL;
    myThread_tcb* ans = arr->array[0];
    for(int i=1; i< arr->size; i++) {
        arr->array[i-1] = arr->array[i];
    }
    arr->size--;
    arr->array[arr->size] = NULL;
    return ans;
}

myThread_tcb* peak_front(myArray_t* arr){
    if(arr->size == 0) return NULL;
    myThread_tcb* ans = arr->array[0];
    return ans;
}

bool isempty_array(myArray_t* arr){
    if(arr->size == 0) return true;
    else return false;
}

// search the thread with the given tID = id
myThread_tcb* search_ele(myArray_t* arr, myThread_t id) {
    if(arr->size == 0) return NULL;
    for(int i=0; i<arr->size; i++) {
        if(arr->array[i]->tid == id) {
            return arr->array[i];
        }
    }
    return NULL;
}

// remove the thread with the given tID = id 
myThread_tcb* remove_ele(myArray_t* arr, myThread_t id) {
    if(arr->size == 0) return NULL;
    int index = -1;
    for(int i=0; i<arr->size; i++) {
        if(arr->array[i]->tid == id) {
            index = i; break;
        }
    }
    if(index<0) return NULL;
    myThread_tcb* ans = arr->array[index];
    for(int i=index+1; i< arr->size; i++) {
        arr->array[i-1] = arr->array[i];
    }
    arr->size--;
    arr->array[arr->size] = NULL;
    return ans;
}

void print_array(myArray_t* arr) {
    for(int i=0; i<arr->size; i++) {
        printf("tid=%d ", arr->array[i]->tid);
    }
    printf("\n");
}

// int main(int argc, char* argv[]) {
//     printf("hello\n");
//     myArray_t* a;
//     a = (myArray_t*)malloc(sizeof(myArray_t));
//     init_array(a);
//     for(int i=0; i<5; i++) {
//         myThread_tcb* th = (myThread_tcb*)malloc(sizeof(myThread_tcb));
//         th->tid = i+1;
//         push_array(a, th);
//     }
//     print_array(a);
//     myThread_tcb* item = remove_ele(a, 2);
//     print_array(a);
//     push_array(a, item);
//     print_array(a);
// }
