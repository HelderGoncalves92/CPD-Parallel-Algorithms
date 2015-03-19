
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int N=100;

int *A;

void OESort(int NN, int *A)
{
    int exch = 1, start = 0, i;
    int temp;
    
    while (exch || start) {
        exch = 0;
        for (i = start; i < NN-1; i+=2) {
            if (A[i] > A[i+1]) {
                temp = A[i]; A[i] = A[i+1]; A[i+1] = temp;
                exch = 1;
            }
        }
        if (start == 0) start = 1;
        else start = 0;
    }
}

void OESort_Parallel_V5(int NN, int *A)
{
    int exch0=0, exch1=1, trips=0, i;
    
    #pragma omp parallel
    {
        while (exch1) {
            
            #pragma omp barrier
            #pragma omp single
            exch0 = exch1 = 0;

            int temp;
            
            #pragma omp for
            for (i=0; i<N-1; i+=2) {
                if (A[i] > A[i+1]) {
                    temp = A[i]; A[i] = A[i+1]; A[i+1] = temp;
                    exch0 = 1;
                }
            }
           
            if(exch0 || !trips){
                #pragma omp for
                for (i=1; i<N-1; i+=2) {
                    if (A[i] > A[i+1]) {
                        temp = A[i]; A[i] = A[i+1]; A[i+1] = temp;
                        exch1 = 1;
                    }
                }
            }
            trips=1;
        }
    }
}

void OESort_Parallel_V4(int NN, int *A)
{
    int exch0, exch1=1, trips=0, i;
    
    while (exch1) {
        exch0 = exch1 = 0;
        
        #pragma omp parallel
        {
            int temp;
            #pragma omp for
            for (i=0; i<N-1; i+=2) {
                if (A[i] > A[i+1]) {
                    temp = A[i]; A[i] = A[i+1]; A[i+1] = temp;
                    exch0 = 1;
                }
            }
            
            if(exch0 || !trips){
                #pragma omp for
                for (i=1; i<N-1; i+=2) {
                    if (A[i] > A[i+1]) {
                        temp = A[i]; A[i] = A[i+1]; A[i+1] = temp;
                        exch1 = 1;
                    }
                }
            }
        }
        trips=1;
    }
}


void OESort_Parallel_V3(int NN, int *A)
{
    int exch, start = 1, i, numThreads;
    
    #pragma omp parallel
    {
        exch = numThreads = omp_get_num_threads();
        int temp;
        while (1) {
            
            #pragma omp for
            for (i = start; i < NN-1; i+=2) {
                if (A[i] > A[i+1]) {
                    temp = A[i]; A[i] = A[i+1]; A[i+1] = temp;
                    
                    exch = numThreads;
                }
            }//Implicit Barrier

            if(exch==0 && start==0) break;

            #pragma omp critical
            exch--;
            
            #pragma omp single //Implicit Barrier
            if (start == 0) start = 1;
            else start = 0;
            
        }
    }
}

void OESort_Parallel_V2(int NN, int *A)
{
    int exch, start = 0, i;
    
    #pragma omp parallel
    {
        exch = omp_get_num_threads();
        int temp;
        while (1) {
            if(exch<=0 && start==0) break;
            
            #pragma omp barrier
            #pragma omp critical
            exch--;

            #pragma omp for 
            for (i = start; i < NN-1; i+=2) {
                if (A[i] > A[i+1]) {
                    temp = A[i]; A[i] = A[i+1]; A[i+1] = temp;
                    #pragma omp critical
                    exch = omp_get_num_threads();
                }
            }
            
            #pragma omp single
            if (start == 0) start = 1;
            else start = 0;
        }
    }
}


void OESort_Parallel_V1(int NN, int *A)
{
    int exch = 1, start = 0, i;
    
    #pragma omp parallel
    {
        int temp;
        while (exch || start) {
          
            #pragma omp barrier
            exch = 0;
            #pragma omp barrier
          
                #pragma omp for
                for (i = start; i < NN-1; i+=2) {
                    if (A[i] > A[i+1]) {
                        temp = A[i]; A[i] = A[i+1]; A[i+1] = temp;
                        exch = 1;
                    }
                }

        #pragma omp single
        if (start == 0) start = 1;
        else start = 0;

        }
    }//Close parallel
}

void init_data()
{
    int i;
    for (i = 0; i < N/2; i++)
        A[i] = i + N/2;
    //	A[i] = rand() % N;
    for (i = N/2; i < N; i++)
        A[i] = i - N/2;
    
}

int main(int argc, char* argv[])
{
    int  j;
    double time=0;
    
    if(argc == 3){
        N=atoi(argv[2]);
        j=atoi(argv[1]);
    }else{
        printf("EXIT\n");
        exit(0);
    }
    
        
    
    A=(int*)malloc(N*sizeof(int));
    
    init_data();
    
  //  for ( i = 0; i < N; i++) printf("%3d ",A[i]);
  //  printf("\n\n");
    
    time=omp_get_wtime();
    //Single
    
    switch(j){
        case 0: OESort(N,A); break;
        case 1: OESort_Parallel_V1(N,A); break;
        case 2: OESort_Parallel_V2(N,A); break;
        case 3: OESort_Parallel_V3(N,A); break;
        case 4: OESort_Parallel_V4(N,A); break;
        case 5: OESort_Parallel_V5(N,A); break;
    }
    time = omp_get_wtime()-time;
    
    //for ( j = 0; j < N; j++) printf("%3d ",A[j]);
    printf("%f\n",time);
    return 0;
}