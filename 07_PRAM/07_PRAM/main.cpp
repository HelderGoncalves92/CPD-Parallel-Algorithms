//
//  main.cpp
//  07_PRAM
//
//  Created by Hélder Gonçalves on 05/05/15.
//  Copyright (c) 2015 Hélder Gonçalves. All rights reserved.
//

#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_THREADS 4
#define NUM 9000000

int N=NUM;  // number of elements in array X
int X[NUM];
int gSum[NUM_THREADS];  // global storage for partial results

int counter=0;
sem_t count_sem;
sem_t barrier_sem;

void *Summation (void *pArg)
{
    int tNum = *((int *) pArg);
    int start, end, i;
    int aux=0, sum=0;
    
    start = (N / NUM_THREADS) * tNum;
    end = (N / NUM_THREADS) * (tNum+1);
    if (tNum == (NUM_THREADS-1)) end = N;
    
    for (i = start+1; i < end; i++)
        X[i] = X[i-1]+X[i];
    
    gSum[tNum] = X[end-1];
    
    /******* Barrier ********/
    sem_wait(&count_sem);
    if(counter == NUM_THREADS-1){
        counter=0;
        sem_post(&count_sem);
        
        //Single
        for(i=0; i<NUM_THREADS; i++){
            sum+=aux;
            aux=gSum[i];
            gSum[i]=sum;
        }
        
        for(i=0; i<NUM_THREADS; i++)
            sem_post(&barrier_sem);
    }else{
        counter++;
        sem_post(&count_sem);
        sem_wait(&barrier_sem);
    }
    
    for (i = start; i < end; i++)
        X[i] += gSum[tNum];

    delete (int *)pArg;
    return NULL;
}

void InitializeArray(int *S, int *N)
{
    int i;
    for (i = 0; i < NUM; i++){
        S[i] = i+1;
    }
}

int main(int argc, char* argv[])
{
    int j;
    double time = omp_get_wtime();
    
    pthread_t tHandles[NUM_THREADS];
    sem_init(&count_sem, 0, 1);
    sem_init(&barrier_sem, 0, 0);
    
    InitializeArray(X,&N);  // get values into A array; not shown
    for (j = 0; j < NUM_THREADS; j++) {
        int *threadNum = (int*)malloc(sizeof (int));
        *threadNum = j;
        pthread_create(&tHandles[j], NULL, Summation, (void *)threadNum);
    }
    
    for (j = 0; j < NUM_THREADS; j++)
        pthread_join(tHandles[j], NULL);
    
    //Get Time
    time = omp_get_wtime() - time;
    
    printf("%f\n",time);
    //printf("\nThe sum of array elements is %d\nTIME:%f\n", sum,time);
    return 0;
}