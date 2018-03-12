#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main(int argc, char **argv) {
    int m, n,l, t, i, j;             // m = A strings, n = B strings , l = string length
    int **a, **b, **distSerial , **distR , **distC, **distRC;    // Tables
    srand(time(NULL));          // init rand
    clock_t start,timeA, timeB, timeC, timeD ;
    float serialT , rowT, columnT, cellT ;


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

//  OpenMP Thread Limit
    omp_set_num_threads(t);


//  Init Tables
    distSerial = malloc(m*sizeof(int*));
    distC = malloc(m*sizeof(int*));
    distR = malloc(m*sizeof(int*));
    distRC = malloc(m*sizeof(int*));

    for (i=0;i<m;i++){
        distSerial[i]=malloc(n*sizeof(int));
        distR[i]=malloc(n*sizeof(int));
        distC[i]=malloc(n*sizeof(int));
        distRC[i]=malloc(n*sizeof(int));
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
    for (int i=0;i<m;i++) {
        for (int j = 0; j < l; j++) {
            a[i][j] = rand() % 2; // random number between 0 and 1
        }
    }


//  init B set
    for (int i=0;i<n;i++) {
        for (int j = 0; j < l; j++) {
            b[i][j] = rand() % 2; // random number between 0 and 1
        }
    }

//  HAMMING

    start = clock();

//    No Parallelization
    for (i=0 ; i<m; i++){
        for (j=0;j<n;j++){
            distSerial[i][j] = hamming(l,a[i],b[j]);
        }
    }

    timeA = clock();

//    Parallelize each row
    #pragma omp parallel for
    for (int i=0 ; i<m; i++){
        for (int j=0;j<n;j++){
            distR[i][j] = hamming(l,a[i],b[j]);
        }
    }

//    #pragma omp barrier
    timeB = clock();

//    Parallelize each column
    #pragma omp parallel for collapse(1)
    for (int j=0 ; j<n; j++){
        for (int i=0;i<m;i++){
            distC[i][j] = hamming(l,a[i],b[j]);
        }
    }
//    #pragma omp barrier
    timeC = clock();

//    Parallelize each cell
    #pragma omp parallel for collapse(2)
    for (int i=0 ; i<m; i++){
        for (int j=0;j<n;j++){
            distRC[i][j] = hamming(l,a[i],b[j]);
        }
    }
//    #pragma omp barrier
    timeD = clock();


//  Check Hamming Distances
    for (i=0 ; i<m; i++){
        for (j=0;j<n;j++){
            if (distSerial[i][j] != distR[i][j]){
                printf("Row Parallelization Data Mismatch");
                return (-1);
            }
            if (distSerial[i][j] != distC[i][j]){
                printf("Column Parallelization Data Mismatch");
                return (-1);
            }
            if (distSerial[i][j] != distRC[i][j]){
                printf("Cell Parallelization Data Mismatch");
                return (-1);
            }
        }
    }



    serialT = (float)((timeA - start) * 1000)/ CLOCKS_PER_SEC;
    rowT = (float)((timeB - timeA) * 1000)/ CLOCKS_PER_SEC;
    columnT = (float)((timeC - timeB) * 1000)/ CLOCKS_PER_SEC;
    cellT = (float)((timeD - timeC) * 1000)/ CLOCKS_PER_SEC;


    printf("\nSerial    Time :%f ms.",serialT);
    printf("\nRow       Time :%f ms.",rowT);
    printf("\nColumn    Time :%f ms.",columnT);
    printf("\nCell      Time :%f ms.",cellT);

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

