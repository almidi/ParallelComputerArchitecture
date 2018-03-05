#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main(int argc, char **argv) {
    int m, n,l, t, i, j;             // m = A strings, n = B strings , l = string length
    int **a, **b, **dist;    // Tables
    srand(time(NULL));          // init rand


//  Check arguments
    if (argc != 5){
        printf("Invalid Arguments");
        return -1;
    }

//  Assign Arguments
    m = atoi(argv[1]);
    n = atoi(argv[2]);
    l = atoi(argv[3]);
    t = atoi(argv[4]);

    printf("Argument: m = %d\n",m);
    printf("Argument: n = %d\n",n);
    printf("Argument: l = %d\n",l);
    printf("Argument: t = %d\n",t);
    printf("-----------------\n\n");

//  Init Tables
    dist = malloc(m*sizeof(int*));
    for (i=0;i<m;i++){
        dist[i]=malloc(n*sizeof(int));
    }

    a = malloc(m*sizeof(int*));
    for (i=0;i<m;i++){
        a[i]=malloc(l*sizeof(int));
    }

    b = malloc(n*sizeof(int*));
    for (i=0;i<n;i++){
        b[i]=malloc(l*sizeof(int));
    }


//  init A set using t or less threads
    if (t<= m){
        omp_set_num_threads(t);
    }
    else{
        omp_set_num_threads(m);
    }
    #pragma omp parallel
    {
        int k = omp_get_thread_num();
        for (int i=k;i<m;i+=t) {
            for (int j = 0; j < l; j++) {
                a[i][j] = rand() % 2; // random number between 0 and 1
            }
        }
    }

//  init B set using t or less threads
    if (t<= n){
        omp_set_num_threads(t);
    }
    else{
        omp_set_num_threads(n);
    }
    #pragma omp parallel
    {
        int k = omp_get_thread_num();
        for (int i=k;i<n;i+=t) {
            for (int j = 0; j < l; j++) {
                b[i][j] = rand() % 2; // random number between 0 and 1
            }
        }
    }

//  Calculate Hamming using t threads
    omp_set_num_threads(t);
    #pragma omp parallel
    {
        int k = omp_get_thread_num();
        for (int i=k ; i<m; i+=t){
            for (int j=0;j<n;j++){
                dist[i][j] = hamming(l,a[i],b[j]);
            }
        }
    }


//  Print Hamming Distances
    for (i=0 ; i<m; i++){
        for (j=0;j<n;j++){
            printf("%d ",dist[i][j]);
        }
        printf("\n");
    }
    return 0;

}


//Hamming Distance Calculator between two arrays of l size
int hamming(int l , int* a, int* b){
    int k = 0;
    for (int i=0;i<l;i++){
        if (a[i] == b[i]){
            k++;
        }
    }
    return k;
}