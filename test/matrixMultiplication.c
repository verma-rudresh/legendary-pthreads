#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/myThread.h"
#include <pthread.h>
#include <math.h>

#define N 3
#define THREADLIMIT 40

int *A, *B, *C;  

int shared;
myThread_mutex_t* mutex1;
double myThreadTotTime; //  total run time of threads plus the context switching time
double myThreadRunTime; //  sum of run time of threads only
double myThreadTimeArray[THREADLIMIT];

void displayMatrix();

void * myThreadFunc(void *arg){
    clock_t t;
    t = clock();
    while(1){
        myThread_mutex_lock(mutex1);
        int n = shared++;
        myThread_mutex_unlock(mutex1);
        if(n>=N*N) break;
        int row = n/N;
        int col = n%N;
        int ans = 0;
        for(int x=0; x<N; x++) ans += A[row*N+x] * B[x*N+col];
        C[row*N+col] = ans;
    }

    
    myThreadRunTime += ((double)clock() - t)/CLOCKS_PER_SEC;
}

void myThreadMultiplication(int numThreads){

    myThreadRunTime = 0;
    myThreadTotTime = 0;

    shared = 0;
    mutex1 = (myThread_mutex_t*)malloc(sizeof(myThread_mutex_t));
    int x = myThread_mutex_init(mutex1);
    clock_t t;
    t = clock();

    int *a = (int*)malloc(sizeof(int)*numThreads);
    myThread_t* threads = (myThread_t *)malloc(sizeof(myThread_t) * numThreads);

    for(int i=0;i<numThreads;i++) a[i] = i+1;

    for(int i=0;i<numThreads;i++){
        myThread_create(&threads[i],myThreadFunc,(void *)&a[i]);
    }
    for(int i=0;i<numThreads;i++){
        int x = myThread_join(threads[i]);
    }

    myThreadTotTime += ((double)clock() - (double)t)/CLOCKS_PER_SEC;

    freeThreads();
    free(a); free(threads);
    free(mutex1);
    // displayMatrix();
}


/* ------------------------------ Pthreads ------------------------------ */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int common = 0;
double pThreadTotTime;
double pThreadRunTime;

void *pThreadFunc(void *arg) {
    clock_t start, end;
    start = clock();

    int ind = * (int *)arg;
    while(1){
        pthread_mutex_lock(&mutex);
        int n = common++;
        pthread_mutex_unlock( &mutex );
        if(n>=N*N) break;
        int row = n/N;
        int col = n%N;
        double ans = 0.0;
        for(int x=0;x<N;x++)ans += A[row*N+x]*B[x*N+col];
        C[row*N+col] = ans;
    }   

    end = clock();
    pThreadRunTime += ((double) (end - start)) / CLOCKS_PER_SEC;
}

void pThreadMultiplication(int numThreads){

    pThreadTotTime = 0;
    pThreadRunTime = 0;
    
    common = 0;

    clock_t start, end;
    start = clock();

    int *a = (int*)malloc(sizeof(int)*numThreads);
    pthread_t* threads = (pthread_t *)malloc(sizeof(pthread_t) * numThreads);

    for(int i=0;i<numThreads;i++) a[i] = i+1;

    for(int i=0;i<numThreads;i++){
        pthread_create(&threads[i], NULL, pThreadFunc, (void *)&a[i]);
    }

    for(int i=0;i<numThreads;i++) {
        pthread_join(threads[i], NULL);
    }

    end = clock();
    pThreadTotTime += ((double) (end - start)) / CLOCKS_PER_SEC;

    free(threads); free(a);
}

//------------------------------------------------------------------------------------------------

double absVal(double x){
    return x>0 ? x:(-1*x);
}

void displayMatrix(){
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            printf(" %d ",C[i*N+j]);
        }
        printf("\n");
    }
}

void plotLineGraph() {
    
    FILE * fp; 
    fp = fopen("./output/data.dat","w");
    for(int i=0;i<THREADLIMIT;i++){
        fprintf(fp, "%d\t%f\n",i+1,1000*myThreadTimeArray[i]);    
    }   
    fclose(fp);

    FILE * GNUpipe = popen ("gnuplot -persistent", "w");
    fprintf(GNUpipe, "set ylabel 'Time (ms)'\n");
    fprintf(GNUpipe, "set xlabel 'No of threads'\n");
    fprintf(GNUpipe,"plot './output/data.dat' using 1:2 title 'Total Execution Time' with lines\n");
    fprintf(GNUpipe, "set term png\n");
    fprintf(GNUpipe, "set output '%s'\n","./output/graph.png");
    fprintf(GNUpipe, "replot\n");
    fclose(GNUpipe);
    
}

int main(void)
{
    A = (int*)malloc(N*N*sizeof(double));
    B = (int*)malloc(N*N*sizeof(double));
    C = (int*)malloc(N*N*sizeof(double));

    for(int i=0;i<N;i++){
            for(int j=0;j<N;j++){
                A[i*N+j] = i+2;
                B[i*N+j] = j+3;
                C[i*N+j] = 0; 
            }
    }
    double myThreadAvgTime = 0;
    double pThreadAvgTime = 0;

    for(int i=1;i<=THREADLIMIT;i++){
        myThreadMultiplication(i);
        pThreadMultiplication(i);
        myThreadTimeArray[i-1] = myThreadTotTime;
        double myt = absVal(myThreadTotTime-myThreadRunTime);
        double pt = absVal(pThreadTotTime-pThreadRunTime);
        myThreadAvgTime += myt/THREADLIMIT;
        pThreadAvgTime += pt/THREADLIMIT;
        printf("For %d threads, mythread Context switch time : %0.2f ms , pthreads Context switch time : %0.2f ms\n",i,1000*myt,1000*pt);
    }

    printf("\nmythread average context switch time : %0.2f ms; pthread average context switch time : %0.2f ms\n",1000*myThreadAvgTime,1000*pThreadAvgTime);

    plotLineGraph();

    free(A);
    free(B);
    free(C);

    return 0;
}