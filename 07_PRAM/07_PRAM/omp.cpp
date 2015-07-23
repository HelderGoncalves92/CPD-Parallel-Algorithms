//
//  main.cpp
//  07_PRAM
//
//  Created by Hélder Gonçalves on 05/05/15.
//  Copyright (c) 2015 Hélder Gonçalves. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


#define NUM 9000000

int NUM_THREADS;
int N=NUM;  // number of elements in array X
int X[NUM];
int* gSum;  // global storage for partial results

int counter=0;


void *Summation ()
{
    int aux=0, sum=0;
    int i;
 
    #pragma omp parallel
    {
        int tNum = omp_get_thread_num();
        
        #pragma omp single
        {
            NUM_THREADS = omp_get_num_threads();
            gSum = (int*)malloc(NUM_THREADS*sizeof(int));
        }
        
        int start, end;
        start = (N / NUM_THREADS) * tNum;
        end = (N / NUM_THREADS) * (tNum+1);
        if (tNum == (NUM_THREADS-1)) end = N;
        
        for (i = start+1; i < end; i++)
            X[i] = X[i-1]+X[i];
        
        gSum[tNum] = X[end-1];
        
        /******* Barrier ********/
        #pragma omp barrier
        
        #pragma omp single
        {
            //Single
            for(i=0; i<NUM_THREADS; i++){
                sum+=aux;
                aux=gSum[i];
                gSum[i]=sum;
            }
        }
    
        for (i = start; i < end; i++)
            X[i] += gSum[tNum];
    }
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
    double time = omp_get_wtime();
    
    InitializeArray(X,&N);  // get values into A array; not shown
    Summation();

    //Get Time
    time = omp_get_wtime() - time;

    printf("%f\n",time);
    //printf("\nThe sum of array elements is %d\nTIME:%f\n", sum,time);
    return 0;
}

