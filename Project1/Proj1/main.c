#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int timeDifference(struct timespec *start, struct timespec *stop) {
    return ((int) ((stop->tv_sec - start->tv_sec) * 1000) + (int) ((stop->tv_nsec - start->tv_nsec) / 1000000));
}


//Hamming Distance Calculator between two arrays of l size
int hamming(int l, int *a, int *b) {
    int k = 0;
    for (int i = 0; i < l; i++) {
        if (a[i] != b[i]) {
            k++;
        }
    }
    return k;
}


int main(int argc, char **argv) {
    int m, n, l, i, j, k;             // m = A strings, n = B strings , l = string length
    int **a, **b, **dist;    // Tables
    struct timespec start, end;

    srand(time(NULL));          // init rand

//  Check arguments
    if (argc != 4) {
        printf("Invalid Arguments");
        return -1;
    }

//  Assign Arguments
    m = atoi(argv[1]);
    n = atoi(argv[2]);
    l = atoi(argv[3]);

//    printf("Argument: m = %d\n",m);
//    printf("Argument: n = %d\n",n);
//    printf("Argument: l = %d\n",l);
//    printf("-----------------\n\n");


    printf("**************************\n");
    printf("********* Serial *********\n");
    printf("**************************\n");
//  Init Tables
    printf("- Initializing");
    fflush(stdout);
    dist = malloc(m * sizeof(int *));
    for (i = 0; i < m; i++) {
        dist[i] = malloc(n * sizeof(int));
    }
    printf(".");
    fflush(stdout);
    a = malloc(m * sizeof(int *));
    for (i = 0; i < m; i++) {
        a[i] = malloc(l * sizeof(int));
    }

    b = malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        b[i] = malloc(l * sizeof(int));
    }
    printf(".");
    fflush(stdout);
//  init A set
    for (i = 0; i < m; i++) {
        for (j = 0; j < l; j++) {
            a[i][j] = rand() % 2; // random number between 0 and 1
        }
    }
    printf(".\n");
    fflush(stdout);
//  init B set
    for (i = 0; i < n; i++) {
        for (j = 0; j < l; j++) {
            b[i][j] = rand() % 2; // random number between 0 and 1
        }
    }

    printf("- Serial Execution: ");
    fflush(stdout);
    clock_gettime(CLOCK_REALTIME, &start);
    k = 0;
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            dist[i][j] = hamming(l, a[i], b[j]);
            k += dist[i][j];
        }
    }
    clock_gettime(CLOCK_REALTIME, &end);
    printf("%d ms\n\n", timeDifference(&start, &end));
    return 0;
}