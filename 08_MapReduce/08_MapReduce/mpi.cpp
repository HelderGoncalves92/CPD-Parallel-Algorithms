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

//Global variables
unsigned int tstart = 1;
unsigned int tend = 250000;
int rank, n_proc, delta, n_elems, last;
int *nElems;


int gcd(int u, int v)
{
    if (v == 0) return u;
    return gcd(v, u % v);
}

void FriendlyNumbers (int start, int end)
{

    int *the_num = new int[last];
    int *num = new int[last];
    int *den = new int[last];
    int i, j, factor, ii, sum, done, n;
    
    double time = MPI_Wtime();
    double twork1 = time, twork2;
    
    
    // -- MAP --
    for (i = start; i <= end; i++) {
        ii = i - start;
        sum = 1 + i;
        the_num[ii] = i;
        done = i;
        factor = 2;
        
        while (factor < done) {
            if ((i % factor) == 0) {
                sum += (factor + (i/factor));
                if ((done = i/factor) == factor) sum -= factor;
            }
            factor++;
        }
        num[ii] = sum; den[ii] = i;
        n = gcd(num[ii], den[ii]);
        num[ii] /= n;
        den[ii] /= n;
    } // end for
    
    twork1 = MPI_Wtime() - twork1;
    
    //Prepare Reduce
    int o=0;
    int toSend = 3*rank;
    int toRecv = 3*(n_proc-rank-1);
    
    MPI_Request reqS[toSend];
    MPI_Request reqR[toRecv];

    //SEND
    for(i=0; i<rank; i++){
        //printf("%d Send to %d: %d\n",rank,i,end-start+1);
        MPI_Isend(num, end-start+1, MPI_INT, i, 0, MPI_COMM_WORLD, &reqS[o++]);
        MPI_Isend(the_num, end-start+1, MPI_INT, i, 1, MPI_COMM_WORLD, &reqS[o++]);
        MPI_Isend(den, end-start+1, MPI_INT, i, 2, MPI_COMM_WORLD, &reqS[o++]);
    }
    
    o=0;
    //Receive
    int aux = 0;
    //if(rank==0){ for(i=0; i<n_proc; i++) printf("%d ",nElems[i]);
     //   printf("\n");}
    
    for(i=rank+1; i<n_proc; i++){
        aux += nElems[i-1];
        
        MPI_Irecv(&num[aux], nElems[i], MPI_INT, i, 0, MPI_COMM_WORLD, &reqR[o++]);
        MPI_Irecv(&the_num[aux], nElems[i], MPI_INT, i, 1, MPI_COMM_WORLD, &reqR[o++]);
        MPI_Irecv(&den[aux], nElems[i], MPI_INT, i, 2, MPI_COMM_WORLD, &reqR[o++]);
    }
    
    MPI_Status statR[toRecv];
    MPI_Waitall(toRecv, reqR, statR);
    
    twork2 = MPI_Wtime();
    
    // -- REDUCE --
    for (i = 0; i < end-start+1; i++) {
        for (j = i+1; j< last; j++) {
            if ((num[i] == num[j]) && (den[i] == den[j]))
                //printf ("%d |%d and %d are FRIENDLY \n",last, the_num[i], the_num[j]);
                o++;
        }
    }
    
    twork2 = MPI_Wtime() - twork2;
    time = MPI_Wtime() - time;
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
    totalTime = MPI_Wtime();
    
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
    

    FriendlyNumbers(fStart, fEnd);
    
    //Get Time and Print
    totalTime = MPI_Wtime() - totalTime; //Tempo total
    if(rank==0)printf("Time: %f\n",totalTime);
    
    MPI_Finalize();
    return 0;
}
