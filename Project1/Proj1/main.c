#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
    int m,n,l,i,j ;             // m = A strings, n = B strings , l = string length
    int **a , **b , **dist ;    // Tables
    srand(time(NULL));          // init rand

//  Check arguments
    if (argc != 4){
        printf("Invalid Arguments");
        return -1;
    }

//  Assign Arguments
    m = atoi(argv[1]);
    n = atoi (argv[2]);
    l = atoi (argv[3]);

    printf("Argument: m = %d\n",m);
    printf("Argument: n = %d\n",n);
    printf("Argument: l = %d\n",l);
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

//  init A set

    for (i=0;i<m;i++){
        for (j=0; j<l; j++){
            a[i][j] = rand() % 2 ; // random number between 0 and 1
        }
    }

//  init B set
    for (i=0;i<n;i++){
        for (j=0; j<l; j++){
            b[i][j] = rand() % 2 ; // random number between 0 and 1
        }
    }
    for (i=0 ; i<m; i++){
        for (j=0;j<n;j++){
            dist[i][j] = hamming(l,a[i],b[j]);
            //printf("%d ",dist[i][j]);
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
