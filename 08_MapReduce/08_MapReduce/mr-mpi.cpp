//
//  main.cpp
//  08_MapReduce
//
//  Created by Hélder Gonçalves on 26/05/15.
//  Copyright (c) 2015 Hélder Gonçalves. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "mapreduce.h"
#include "keyvalue.h"
using namespace MAPREDUCE_NS;


//Global variables
unsigned int tstart = 1;
unsigned int tend = 250000;
int rank, n_proc, delta, n_elems, last;
int start, end;
int *nElems;


int gcd(int u, int v)
{
    if (v == 0) return u;
    return gcd(v, u % v);
}

void FriendlyNumbers (int itask, KeyValue *kv, void* ptr)
{
    char key[30], value[20];
    int lenKey, lenValue;
    int num, den;
    int i, factor, sum, done, n;

    // -- MAP --
    for (i = start; i <= end; i++) {
        sum = 1 + i;
        done = i;
        factor = 2;
        
        while (factor < done) {
            if ((i % factor) == 0) {
                sum += (factor + (i/factor));
                if ((done = i/factor) == factor) sum -= factor;
            }
            factor++;
        }
        num = sum; den = i;
        n = gcd(num, den);
        num /= n;
        den /= n;
        
        lenKey = sprintf(key, "%d %d", num, den);
        lenValue = sprintf(value, "%d", i);
        kv->add(key, lenKey+1, value, lenValue+1);
    } // end for
}


void myReduce(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes, KeyValue *kv, void *ptr){
    int sum=0;
    
    if(nvalues>1){
        printf ("Key '%s':\t", key);
        for(int i=0; i<nvalues; i++){
            printf("%s ", &multivalue[sum]);
            sum += valuebytes[i];
        }
        std::cout << "\n";
    }
}


int main(int argc, char **argv)
{
    double totalTime;
    int auxStart=0, auxEnd, remain, i, j;
    int fStart=0, fEnd=0;
    float perc, plus = 0.05;
    
    //Inicializar MPI
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&n_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    
    nElems = (int*)malloc(n_proc*sizeof(int));
    perc = 1.0/(float)n_proc + plus;
    remain = n_elems = tend - tstart + 1;
    
    j=n_proc;
    auxEnd = tstart;
    for(i=0; i<n_proc; i++){
        auxStart = auxEnd;
        auxEnd = auxStart + (remain * perc);
        
        if(i != 0)
            auxStart++;
        
        nElems[i] = (auxEnd-auxStart) + 1;
        remain -= auxEnd - auxStart;
        
        if(rank == i){
            fStart=auxStart;
            fEnd = auxEnd;
        }
        
        j--;
        perc = 1.0/(float)j + plus;
    }

    
    auxEnd = tend;
    nElems[n_proc-1] = (auxEnd-auxStart) + 1;
    
    last = 0;
    for(i=n_proc-1; i>=rank; i--)
        last  += nElems[i];
    
    if(rank == n_proc-1)
        fEnd = tend;
    
    
    start = fStart;
    end =  fEnd;
    
    totalTime = MPI_Wtime();
    
    MapReduce *mr = new MapReduce(MPI_COMM_WORLD);
    mr->map(n_proc, &FriendlyNumbers, NULL);
    mr->collate(NULL);
    mr->reduce(&myReduce, NULL);

    totalTime = MPI_Wtime() - totalTime;
    if(rank==0)printf("Time: %f\n",totalTime);
    delete mr;
    MPI_Finalize();
    return 0;
}
