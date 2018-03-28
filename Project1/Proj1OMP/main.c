#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int hamming(int l, u_int8_t *a, u_int8_t *b);

int timedif(struct timespec *start, struct timespec *stop);


int main(int argc, char **argv) {
    int m, n, l, t, i, j, k, distSumR = 0, distSumC = 0, distSumH = 0;             // m = A strings, n = B strings , l = string length
    __uint8_t **a, **b;
    int **distSerial, **distR, **distC, **distCH;    // Tables
    srand(time(NULL));          // init rand
    struct timespec startTime, endTime;
    int serialT, rowT, cellT, charT;
    omp_lock_t **locks;


//  Check arguments
    if (argc != 5) {
        printf("Invalid Arguments");
        return -1;
    }

//  Assign Arguments
    m = atoi(argv[1]);
    n = atoi(argv[2]);
    l = atoi(argv[3]);
    t = atoi(argv[4]);

//    printf("\nArguments");
//    printf("\n-------------------");
//    printf("\n     m: %d",m);
//    printf("\n     n: %d",n);
//    printf("\n     l: %d",l);
//    printf("\n     t: %d\n",t);
    printf("**************************\n");
    printf("********* OpenMP *********\n");
    printf("**************************\n");

    printf("- Initialization");
    fflush(stdout);
//  OpenMP Thread Limit
    omp_set_num_threads(t);


//  Allocate Tables
//    distSerial = malloc(m * sizeof(int **));
    distC = malloc(m * sizeof(int **));
    distR = malloc(m * sizeof(int **));
    distCH = malloc(m * sizeof(int **));
    locks = malloc(m * sizeof(omp_lock_t **));

    for (i = 0; i < m; i++) {
//        distSerial[i] = malloc(n * sizeof(int));
        distR[i] = malloc(n * sizeof(int));
        distC[i] = malloc(n * sizeof(int));
        distCH[i] = malloc(n * sizeof(int));
        locks[i] = malloc(n * sizeof(omp_lock_t *));
    }

    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            distR[i][j] = 0;
            distC[i][j] = 0;
            distCH[i][j] = 0;
        }
    }
    printf(".");
    fflush(stdout);
    a = malloc(m * sizeof(int *));
    for (i = 0; i < m; i++) {
        a[i] = malloc(l * sizeof(u_int8_t));
    }

    b = malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        b[i] = malloc(l * sizeof(u_int8_t));
    }
    printf(".");
    fflush(stdout);
//  init A set
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < l; j++) {
            a[i][j] = rand() % 2; // random number between 0 and 1
        }
    }


//  init B set
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < l; j++) {
            b[i][j] = rand() % 2; // random number between 0 and 1
        }
    }
    printf(".\n");
    fflush(stdout);
//  init distCH and thread locks
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            distCH[i][j] = 0;
            omp_init_lock(&locks[i][j]);
        }
    }


//  HAMMING
//  No Parallelization
//    printf("- Serial Execution: ");
//    fflush(stdout);
//    clock_gettime(CLOCK_REALTIME, &startTime);
//    for (i = 0; i < m; i++) {
//        for (j = 0; j < n; j++) {
//            distSerial[i][j] = hamming(l, a[i], b[j]);
//        }
//    }
//    clock_gettime(CLOCK_REALTIME, &endTime);
//    printf("%d ms\n", timedif(&startTime, &endTime));


//  Parallelize each row
//  Each task take a string from array "a" and
//  processes it with every string from array "b".
    printf("- Parallel by Row Execution: ");
    fflush(stdout);
    clock_gettime(CLOCK_REALTIME, &startTime);
#pragma omp parallel for schedule(dynamic) private(j) shared(a, b, distR)
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            distR[i][j] = hamming(l, a[i], b[j]);
            distSumR += distR[i][j];
        }
    }
    clock_gettime(CLOCK_REALTIME, &endTime);
    printf("%d ms\n", timedif(&startTime, &endTime));

//  Parallelize each cell
//  Each task takes a  string from array "a" and
//  processes it with a string from array "b".
    printf("- Parallel by Cell Execution: ");
    fflush(stdout);
    clock_gettime(CLOCK_REALTIME, &startTime);
#pragma omp parallel for schedule(dynamic) collapse(2) private(i, j) shared(a, b, distC)
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            distC[i][j] = hamming(l, a[i], b[j]);
            distSumC += distC[i][j];
        }
    }
    clock_gettime(CLOCK_REALTIME, &endTime);
    printf("%d ms\n", timedif(&startTime, &endTime));

//  Parallelize each string
//  Each task takes a character from a string from array "a" and processes it
//  with the corresponding character from a string in array "b".
    printf("- Parallel by Char Execution: ");
    fflush(stdout);
    clock_gettime(CLOCK_REALTIME, &startTime);
#pragma omp parallel for private(i, j, k) shared(a, b, distCH, locks)
    for (i = 0; i < m; i++) {
#pragma omp parallel for private(j, k) shared(a, b, distCH, locks)
        for (j = 0; j < n; j++) {
#pragma omp parallel for private(k) shared(a, b, distCH, locks)
            for (k = 0; k < l; k++) {
                if (a[i][k] != b[j][k]) {
                    //set lock for string
                    omp_set_lock(&locks[i][j]);
                    //increase strings hamming distance
                    //#pragma omp atomic update
                    distCH[i][j]++;
                    //unset lock for string
                    omp_unset_lock(&locks[i][j]);
                }
            }
        }
    }
    clock_gettime(CLOCK_REALTIME, &endTime);
    printf("%d ms\n", timedif(&startTime, &endTime));
    printf("\n");
//    for (i = 0; i < m; i++) {
//        for (j = 0; j < n; j++) {
//            distSumH += distCH[i][j];
//        }
//    }

//  Check Hamming Distances
//    for (i = 0; i < m; i++) {
//        for (j = 0; j < n; j++) {
//            if (distSerial[i][j] != distR[i][j]) {
//                printf("Row Parallelization Data Mismatch");
//                return (-1);
//            }
//            if (distSerial[i][j] != distC[i][j]) {
//                printf("Cell Parallelization Data Mismatch");
//                return (-1);
//            }
//            if (distSerial[i][j] != distCH[i][j]) {
//                printf("Character Parallelization Data Mismatch");
//                return (-1);
//            }
//        }
//    }
    return 0;
}

//Hamming Distance Calculator between two arrays of l size
int hamming(int l, u_int8_t *a, u_int8_t *b) {
    int k = 0;
    for (int i = 0; i < l; i++) {
        if (a[i] != b[i]) {
            k++;
        }
    }
    return k;
}


int timedif(struct timespec *start, struct timespec *stop) {
    return ((int) ((stop->tv_sec - start->tv_sec) * 1000) + (int) ((stop->tv_nsec - start->tv_nsec) / 1000000));
}



