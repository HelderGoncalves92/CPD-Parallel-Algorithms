/* File:
 *     omp_mat_vect_rand_split.c
 *
 * Purpose:
 *     Computes a parallel matrix-vector product.  Matrix
 *     is distributed by block rows.  Vectors are distributed by
 *     blocks.  This version uses a random number generator to
 *     generate A and x.  There is some optimization.
 *
 * Compile:
 *    gcc -g -Wall -fopenmp -o omp_mat_vect_rand_split
 *          omp_mat_vect_rand_split.c
 * Run:
 *    ./omp_mat_vect_rand_split <thread_count> <m> <n>
 *
 * Input:
 *     None unless compiled with DEBUG flag.
 *     With DEBUG flag, A, x
 *
 * Output:
 *     y: the product vector
 *     Elapsed time for the computation
 *
 * Notes:
 *     1.  Storage for A, x, y is dynamically allocated.
 *     2.  Number of threads (thread_count) should evenly divide both
 *         m and n.  The program doesn't check for this.
 *     3.  We use a 1-dimensional array for A and compute subscripts
 *         using the formula A[i][j] = A[i*n + j]
 *     4.  Distribution of A, x, and y is logical:  all three are
 *         globally shared.
 *     5.  DEBUG compile flag will prompt for input of A, x, and
 *         print y
 *
 * IPP:  Exercise 5.12
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <omp.h>
#include "timer.h"

double* A;
double* x;
double* y;
int m, n, thread_count;

/* Serial functions */
void Get_args(int argc, char* argv[], int* thread_count_p,
              int* m_p, int* n_p);
void Usage(char* prog_name);
void Gen_matrix(double A[], int m, int n);
void Read_matrix(char* prompt, double A[], int m, int n);
void Gen_vector(double x[], int n);
void Read_vector(char* prompt, double x[], int n);
void Print_matrix(char* title, double A[], int m, int n);
void Print_vector(char* title, double y[], double m);

/* Parallel function */
void* Omp_mat_vect(void* id);

/*------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    double start, finish, elapsed;
    int i;
    
    Get_args(argc, argv, &thread_count, &m, &n);
    
    A = (double*)malloc(m*n*sizeof(double));
    x = (double*)malloc(n*sizeof(double));
    y = (double*)malloc(m*sizeof(double));
    
#  ifdef DEBUG
    Read_matrix("Enter the matrix", A, m, n);
    Print_matrix("We read", A, m, n);
    Read_vector("Enter the vector", x, n);
    Print_vector("We read", x, n);
#  else
    Gen_matrix(A, m, n);
    /* Print_matrix("We generated", A, m, n); */
    Gen_vector(x, n);
    /* Print_vector("We generated", x, n); */
#  endif
    
    
    //Threads Start
    pthread_t tHandles[thread_count];
    

    GET_TIME(start);
    #pragma pomp inst begin(enum)
    
    
    for (i=0; i<thread_count ; i++) {
        short *threadNum = (short*)malloc(sizeof (short));
        *threadNum = i;
        pthread_create(&tHandles[i], NULL, Omp_mat_vect, (void *)threadNum);
    }
    
    //Threads Join
    for (i = 0; i < thread_count; i++)
        pthread_join(tHandles[i], NULL);
    
    #pragma pomp inst end(enum)
    GET_TIME(finish);
    
    
    elapsed = finish - start;
    printf("Elapsed time = %e seconds\n", elapsed);
    
#  ifdef DEBUG
    Print_vector("The product is", y, m);
#  else
    /* Print_vector("The product is", y, m); */
#  endif
    
    free(A);
    free(x);
    free(y);
    
    return 0;
}  /* main */


/*------------------------------------------------------------------
 * Function:  Get_args
 * Purpose:   Get command line args
 * In args:   argc, argv
 * Out args:  thread_count_p, m_p, n_p
 */
void Get_args(int argc, char* argv[], int* thread_count_p,int* m_p, int* n_p)  {
    
    if (argc != 4) Usage(argv[0]);
    *thread_count_p = strtol(argv[1], NULL, 10);
    *m_p = strtol(argv[2], NULL, 10);
    *n_p = strtol(argv[3], NULL, 10);
    if (*thread_count_p <= 0 || *m_p <= 0 || *n_p <= 0) Usage(argv[0]);

}  /* Get_args */

/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   print a message showing what the command line should
 *            be, and terminate
 * In arg :   prog_name
 */
void Usage (char* prog_name) {
    fprintf(stderr, "usage: %s <thread_count> <m> <n>\n", prog_name);
    exit(0);
}  /* Usage */

/*------------------------------------------------------------------
 * Function:    Read_matrix
 * Purpose:     Read in the matrix
 * In args:     prompt, m, n
 * Out arg:     A
 */
void Read_matrix(char* prompt, double A[], int m, int n) {
    int             i, j;
    
    printf("%s\n", prompt);
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            scanf("%lf", &A[i*n+j]);
}  /* Read_matrix */

/*------------------------------------------------------------------
 * Function: Gen_matrix
 * Purpose:  Use the random number generator random to generate
 *    the entries in A
 * In args:  m, n
 * Out arg:  A
 */
void Gen_matrix(double A[], int m, int n) {
    int i, j;
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            A[i*n+j] = random()/((double) RAND_MAX);
}  /* Gen_matrix */

/*------------------------------------------------------------------
 * Function: Gen_vector
 * Purpose:  Use the random number generator random to generate
 *    the entries in x
 * In arg:   n
 * Out arg:  A
 */
void Gen_vector(double x[], int n) {
    int i;
    for (i = 0; i < n; i++)
        x[i] = random()/((double) RAND_MAX);
}  /* Gen_vector */

/*------------------------------------------------------------------
 * Function:        Read_vector
 * Purpose:         Read in the vector x
 * In arg:          prompt, n
 * Out arg:         x
 */
void Read_vector(char* prompt, double x[], int n) {
    int   i;
    
    printf("%s\n", prompt);
    for (i = 0; i < n; i++)
        scanf("%lf", &x[i]);
}  /* Read_vector */


/*------------------------------------------------------------------
 * Function:  Omp_mat_vect
 * Purpose:   Multiply an mxn matrix by an nx1 column vector
 * In args:   A, x, m, n, thread_count
 * Out arg:   y
 */
void* Omp_mat_vect(void* vID) {
    double temp;
    int i, j, id, auxStart, auxEnd, chunk;
    
    id = *((short *) vID);
    chunk = m/thread_count;
    auxStart = id*chunk;
    
    if(id == thread_count-1)    auxEnd = m;
    else auxEnd = (id+1)*chunk;
    
    for (i = auxStart; i < auxEnd; i++) {
        y[i] = 0.0;
        for (j = 0; j < n; j++) {
            temp = A[i*n+j]*x[j];
            y[i] += temp;
        }
    }
    
    return NULL;
}  /* Omp_mat_vect */


/*------------------------------------------------------------------
 * Function:    Print_matrix
 * Purpose:     Print the matrix
 * In args:     title, A, m, n
 */
void Print_matrix( char* title, double A[], int m, int n) {
    int   i, j;
    
    printf("%s\n", title);
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++)
            printf("%4.1f ", A[i*n + j]);
        printf("\n");
    }
}  /* Print_matrix */


/*------------------------------------------------------------------
 * Function:    Print_vector
 * Purpose:     Print a vector
 * In args:     title, y, m
 */
void Print_vector(char* title, double y[], double m) {
    int   i;
    
    printf("%s\n", title);
    for (i = 0; i < m; i++)
        printf("%4.1f ", y[i]);
    printf("\n");
}  /* Print_vector */