#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int hamming(int l , int* a, int* b);
int timedif(struct timespec *start, struct timespec *stop);


int main(int argc, char **argv) {
    int m, n,l, t, i, j, k;             // m = A strings, n = B strings , l = string length
    int **a, **b, **distSerial , **distR , **distC, **distH;    // Tables
    srand(time(NULL));          // init rand
    struct timespec start,timeA, timeB, timeC, timeD ;
    int  serialT , rowT, columnT, cellT ;


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

    printf("\nArguments");
    printf("\n-------------------");
    printf("\n     m: %d",m);
    printf("\n     n: %d",n);
    printf("\n     l: %d",l);
    printf("\n     t: %d\n",t);

//  OpenMP Thread Limit
    omp_set_num_threads(t);



//  Init Tables
    distSerial = malloc(m*sizeof(int*));
    distC = malloc(m*sizeof(int*));
    distR = malloc(m*sizeof(int*));
    distH = malloc(m*sizeof(int*));

    for (i=0;i<m;i++){
        distSerial[i]=malloc(n*sizeof(int));
        distR[i]=malloc(n*sizeof(int));
        distC[i]=malloc(n*sizeof(int));
        distH[i]=malloc(n*sizeof(int));
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

    clock_gettime(CLOCK_REALTIME,&start);

//  No Parallelization
    for (i=0 ; i<m; i++){
        for (j=0;j<n;j++){
            distSerial[i][j] = hamming(l,a[i],b[j]);
        }
    }

    clock_gettime(CLOCK_REALTIME,&timeA);

//  Parallelize each row
//  Each task take a string from array "a" and
//  processes it with every string from array "b".
    #pragma omp parallel for schedule(dynamic) private(j)
    for (i=0 ; i<m; i++){
        for (j=0;j<n;j++){
            distR[i][j] = hamming(l,a[i],b[j]);
        }
    }

    clock_gettime(CLOCK_REALTIME,&timeB);

//  Parallelize each cell
//  Each task takes a  string from array "a" and
//  processes it with a string from array "b".
    #pragma omp parallel for collapse(2) private(i,j)
    for (i=0 ; i<m; i++){
        for (j=0;j<n;j++){
            distC[i][j] = hamming(l,a[i],b[j]);
        }
    }

    clock_gettime(CLOCK_REALTIME,&timeC);

//  Parallelize each string
//  Each task takes a character from a string from array "a" and processes it
//  with the corresponding character from a string in array "b".
    #pragma omp parallel for collapse(3) private(i,j,k)
    for (i=0 ; i<m; i++){
        for (j=0;j<n;j++){
            for (k=0;k<l;k++){
                if (a[i][k] != b[j][k]){

                    distH[i][j]++;
                }
            }
        }
    }

    clock_gettime(CLOCK_REALTIME,&timeD);

//  Check Hamming Distances
    for (i=0 ; i<m; i++){
        for (j=0;j<n;j++){
            if (distSerial[i][j] != distR[i][j]){
                printf("Row Parallelization Data Mismatch");
                return (-1);
            }
            if (distSerial[i][j] != distC[i][j]){
                printf("Cell Parallelization Data Mismatch");
                return (-1);
            }
            if (distSerial[i][j] != distH[i][j]){
                printf("String Parallelization Data Mismatch");
                return (-1);
            }
        }
    }

//  Calculate excecution times
    serialT =  timedif(&start,&timeA);
    rowT = timedif(&timeA,&timeB);
    columnT = timedif(&timeB,&timeC);
    cellT = timedif(&timeC,&timeD);

//  Print excecution times
    printf("\nComputation Times");
    printf("\n-------------------");
    printf("\nSerial: %d ms",serialT);
    printf("\n   Row: %d ms",rowT);
    printf("\nColumn: %d ms",columnT);
    printf("\n  Char: %d ms\n",cellT);

    return 0;


}

//Hamming Distance Calculator between two arrays of l size
int hamming(int l , int* a, int* b){
    int k = 0;
    for (int i=0;i<l;i++){
        if (a[i] != b[i]){
            k++;
        }
    }
    return k;
}


int timedif(struct timespec *start, struct timespec *stop){
    return((int)((stop->tv_sec - start->tv_sec)*1000)+(float)((stop->tv_nsec - start->tv_nsec)/1000000));
}



