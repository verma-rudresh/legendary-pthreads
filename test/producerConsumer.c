#include <unistd.h>

#include "../include/myThread.h"
 
int N;
int limit;
int M;
int itemsProduced;
int itemsConsumed;

int in = 0;
int out = 0;
int* buffer;
myThread_mutex_t m1;

volatile int count = 0;

void *producer(void *pno)
{   
    int item;
    while(1){
        item = rand()%100; // Produce an random item
        if(count==N) {
            continue;
        }

        myThread_mutex_lock(&m1);
    
        if(itemsConsumed >= limit || itemsProduced >= limit){
            myThread_mutex_unlock(&m1);
            myThread_exit();
        }
        buffer[in] = item;
        itemsProduced++; count++;
        printf("Producer %d: Inserted Item %d at container %d\n", *((int *)pno),buffer[in],in);
        in = (in+1)%N;
        myThread_mutex_unlock(&m1);
    }
}
void *consumer(void *cno)
{   
    while(1){
        if(count == 0) {
            continue;
        }
        myThread_mutex_lock(&m1);
    
        if(itemsConsumed >= limit || itemsProduced >= limit){
            myThread_mutex_unlock(&m1);
            myThread_exit();
        }
        int item = buffer[out];
        itemsConsumed++; count--;
        printf("Consumer %d: Removed Item %d from container %d\n",*((int *)cno),item, out);
        out = (out+1)%N;
        myThread_mutex_unlock(&m1);
    }
}


int main(int argc, char* argv[]) 
{
    
    N = argc < 3 ? 10 : atoi(argv[1]);
    M = argc < 3 ? 5 : atoi(argv[2]);

    limit = 10*N;

    itemsConsumed = 0;
    itemsProduced = 0;

    buffer = malloc(N * sizeof(int));

    int a[M*2];
    for(int i=0; i<2*M; i++) a[i] = i+1;

    for(int i = 0; i < M; i++) {
        myThread_create(&a[i],(void *)producer,(void *)&a[i]);
    }
    for(int i = 0; i < M; i++) {
        myThread_create(&a[i+M],(void *)consumer,(void *)&a[i+M]);
    }

    for(int i = 0; i < M; i++) {
        myThread_join(a[i]);
    }
    for(int i = 0; i < M; i++) {
        myThread_join(a[i+M]);
    }

    printf("Total items produced : %d\n",itemsProduced);
    printf("Total items consumed : %d\n",itemsConsumed);

    free(buffer);

    return 0;
    
}
