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

int N=NUM;  // number of elements in array X
int X[NUM];

void *Summation()
{
    for (int i = 1; i < N; i++)
        X[i] = X[i-1]+X[i];

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
    //printf("\nThe sum of array elements is %d\nTIME:%f\n", X[N-1],time);
    return 0;
}