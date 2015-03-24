/* File:    bubble.c
 *
 * Purpose: Use bubble sort to sort a list of ints.
 *
 * Compile: gcc -g -Wall -o bubble bubble.c
 * Usage:   bubble <n> <g|i>
 *             n:   number of elements in list
 *            'g':  generate list using a random number generator
 *            'i':  user input list
 *
 * Input:   list (optional)
 * Output:  sorted list
 *
 * IPP:     Section 3.7.1 (pp. 127 and ff.) and Section 5.6.1
 *          (pp. 232 and ff.)
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <omp.h>

/* For random list, 0 <= keys < RMAX */
const int RMAX = 100;
int  ordered = 0, n, list_length;
int* sectionsOffset;

int num_threads;
pthread_mutex_t *mutex;


void Usage(char* prog_name);
void Get_args(int argc, char* argv[], char* g_i_p);
void Generate_list(int a[]);
void Print_list(int a[], char* title);
void Read_list(int a[]);
void* Bubble_sort(void*);
void Bubble_sort_Single(int a[]);
int testeOrder(int a[]);

/*-----------------------------------------------------------------*/



pthread_t* preparePthreads(){
    
    pthread_t* thread_handles;
    int i, stride;
    
    //Pthreads - Memory Alloc
    thread_handles = malloc(num_threads * sizeof(pthread_t));
    mutex = malloc((num_threads+1) * sizeof(pthread_mutex_t));
    
    for(i=0; i <= num_threads; i++){
        pthread_mutex_init(&mutex[i], NULL);
    }
    
    //Prepare Sections
    stride = n / num_threads;
    sectionsOffset = (int*)malloc(num_threads*sizeof(int));
    
    for(i=0; i < num_threads;i++)
        sectionsOffset[i] = (i+1)*stride;
    
    return thread_handles;
}



int main(int argc, char* argv[]) {
    
    char g_i;
    int *a, thread;
    
    pthread_t *thread_handles = NULL;
    
    //Read args and Generate Array
    Get_args(argc, argv, &g_i);
    a = (int*) malloc(n*sizeof(int));
    if (g_i == 'g') {
        Generate_list(a);
        //Print_list(a, n, "Before sort");
    } else {
        Read_list(a);
    }
    
    if (num_threads == 1) {
        double time = omp_get_wtime();
        Bubble_sort_Single(a);
        printf("%f\n",omp_get_wtime()-time);
    }else{
        
        //Init all configurations
        thread_handles = preparePthreads();
        
        double time = omp_get_wtime();
        
        //Pthreads Start
        for(thread=0; thread < num_threads; thread++){
            pthread_create(&thread_handles[thread], NULL, Bubble_sort, (void*)a);
        }
        
        //Pthreads Join
        for(thread=0; thread < num_threads; thread++){
            pthread_join(thread_handles[thread], NULL);
        }
        printf("%f\n",omp_get_wtime()-time);
    }
    
    //To test if was ordered
    if(testeOrder(a)) printf("OK!!\n"); else printf("NÂO OK!!\n");
   // Print_list(a, n, "After sort");
    
    free(a);
    return 0;
}  /* main */


/*-----------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Summary of how to run program
 */
void Usage(char* prog_name) {
    fprintf(stderr, "usage:   %s <t> <n> <g|i>\n", prog_name);
    fprintf(stderr, "   t:   number of threads\n");
    fprintf(stderr, "   n:   number of elements in list\n");
    fprintf(stderr, "  'g':  generate list using a random number generator\n");
    fprintf(stderr, "  'i':  user input list\n");
}  /* Usage */


/*-----------------------------------------------------------------
 * Function:  Get_args
 * Purpose:   Get and check command line arguments
 * In args:   argc, argv
 * Out args:  n_p, g_i_p
 */
void Get_args(int argc, char* argv[], char* g_i_p) {
    if (argc != 4 ) {
        Usage(argv[0]);
        exit(0);
    }
    
    num_threads = atoi(argv[1]);
    n = atoi(argv[2]);
    *g_i_p = argv[3][0];
    
    if (num_threads <= 0 || n <= 0 || (*g_i_p != 'g' && *g_i_p != 'i') ) {
        Usage(argv[0]);
        exit(0);
    }
}  /* Get_args */


/*-----------------------------------------------------------------
 * Function:  Generate_list
 * Purpose:   Use random number generator to generate list elements
 * In args:   n
 * Out args:  a
 */
void Generate_list(int a[]) {
    int i;
    
    srandom(0);
    for (i = 0; i < n; i++)
        a[i] = random() % RMAX;
}  /* Generate_list */


/*-----------------------------------------------------------------
 * Function:  Print_list
 * Purpose:   Print the elements in the list
 * In args:   a, n
 */
void Print_list(int a[], char* title) {
    int i;
    
    printf("%s:\n", title);
    for (i = 0; i < n; i++)
        printf("%d ", a[i]);
    printf("\n\n");
}  /* Print_list */


/*-----------------------------------------------------------------
 * Function:  Read_list
 * Purpose:   Read elements of list from stdin
 * In args:   n
 * Out args:  a
 */
void Read_list(int a[]) {
    int i;
    
    printf("Please enter the elements of the list\n");
    for (i = 0; i < n; i++)
        scanf("%d", &a[i]);
}  /* Read_list */

int testeOrder(int a[]){
    int i;
    for(i=0; i<n-1;i++){
        if(a[i]>a[i+1]) return 0;
    }
    return 1;
}


/*-----------------------------------------------------------------
 * Function:     Bubble_sort
 * Purpose:      Sort list using bubble sort
 * In args:      n
 * In/out args:  a
 */
void* Bubble_sort(void* array  /* in/out */) {
    
    int *a = (int*)array;
    list_length=n;
    
    
    int i, temp;
    int section;
    int change;

    while(!ordered){
        section = change = 0;
        pthread_mutex_lock(&mutex[0]);
        
        for (i = 0; i < list_length-1; i++){
            if (a[i] > a[i+1]) {
                temp = a[i];
                a[i] = a[i+1];
                a[i+1] = temp;
                change=1;
                
            }
            if(i == sectionsOffset[section]){
                pthread_mutex_lock(&mutex[++section]);
                pthread_mutex_unlock(&mutex[section-1]);
            }
        }
        if(change == 0) ordered=1;
        list_length--;

        pthread_mutex_unlock(&mutex[section]);
    }
    return NULL;

}  /* Bubble_sort */

void Bubble_sort_Single(int  a[]  /* in/out */) {
    int list_length, i, temp;
    
    for (list_length = n; list_length >= 2; list_length--)
        for (i = 0; i < list_length-1; i++)
            if (a[i] > a[i+1]) {
                temp = a[i];
                a[i] = a[i+1];
                a[i+1] = temp;
            }
    
}  /* Bubble_sort */

