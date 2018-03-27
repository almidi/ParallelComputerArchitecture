#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


struct charThreadData {
    int tid;
    int row;
    int col;
    int index;
} *charTD;
int **a, **b, m, n, l, t;
int **distSerial, **distR, **distC, **distCH;    // Tables

//Threads
pthread_t *pthread;
pthread_mutex_t counterMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t **cellMutex;

struct timespec start, end;


int timeDifference(struct timespec *start, struct timespec *stop) {
    return ((int) ((stop->tv_sec - start->tv_sec) * 1000) + (int) ((stop->tv_nsec - start->tv_nsec) / 1000000));
}


void initTables() {
    printf("Initializing\n");
    distSerial = malloc(m * sizeof(int *));
    distC = malloc(m * sizeof(int *));
    distR = malloc(m * sizeof(int *));
    distCH = malloc(m * sizeof(int *));
    pthread = malloc(t * sizeof(struct pthread_t *));
    cellMutex = malloc(m * sizeof(pthread_mutex_t *));

    for (int i = 0; i < m; i++) {
        distSerial[i] = malloc(n * sizeof(int));
        distR[i] = malloc(n * sizeof(int));
        distC[i] = malloc(n * sizeof(int));
        distCH[i] = malloc(n * sizeof(int));
        cellMutex[i] = malloc(n * sizeof(pthread_mutex_t));
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            distR[i][j] = 0;
            distC[i][j] = 0;
            distCH[i][j] = 0;
            pthread_mutex_init(&cellMutex[i][j], NULL);
        }
    }

    a = malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        a[i] = malloc(l * sizeof(int));
    }

    b = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        b[i] = malloc(l * sizeof(int));
    }

    //Get random seed based on current time for the rand function
    srand((unsigned int) time(NULL));
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
}


//Hamming Distance Calculator between two arrays of l size
int hamming(const int *a, const int *b) {
    int k = 0;
    for (int i = 0; i < l; i++) {
        if (a[i] != b[i]) {
            k++;
        }
    }
    return k;
}


void serialHamming() {
    printf("- Serial Execution: ");
    fflush(stdout);
    clock_gettime(CLOCK_REALTIME, &start);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            distSerial[i][j] = hamming(a[i], b[j]);
        }
    }
    clock_gettime(CLOCK_REALTIME, &end);
    printf("%d ms\n", timeDifference(&start, &end));
}


u_int64_t taskCount, taskIndex;


void *rowThread(void *threadarg) {
    int row;
    pthread_mutex_lock(&counterMutex);
    while (taskIndex < taskCount) {
        row = taskIndex++;
        pthread_mutex_unlock(&counterMutex);
        for (int i = 0; i < n; i++) {
            distR[row][i] = hamming(a[row], b[i]);
        }
        pthread_mutex_lock(&counterMutex);
    }
    pthread_mutex_unlock(&counterMutex);
}


void *cellThread(void *threadarg) {
    int row, col;
    pthread_mutex_lock(&counterMutex);
    while (taskIndex < taskCount) {
        row = taskIndex / n;
        col = taskIndex % n;
        taskIndex++;
        pthread_mutex_unlock(&counterMutex);
        distC[row][col] = hamming(a[row], b[col]);
        pthread_mutex_lock(&counterMutex);
    }
    pthread_mutex_unlock(&counterMutex);
}


void *charThread(void *threadarg) {
    int row, col, index;
    pthread_mutex_lock(&counterMutex);
    while (taskIndex < taskCount) {
        row = taskIndex / l / n;
        col = taskIndex / l % n;
        index = taskIndex % l;
        taskIndex++;
        pthread_mutex_unlock(&counterMutex);
        if (a[row][index] != b[col][index]) {
            pthread_mutex_lock(&cellMutex[row][col]);
            distCH[row][col]++;
            pthread_mutex_unlock(&cellMutex[row][col]);
        }
        pthread_mutex_lock(&counterMutex);
    }
    pthread_mutex_unlock(&counterMutex);
}


void threadSpawner(char *message, void *functionPointer, u_int64_t taskCountArg) {
    printf("%s", message);
    fflush(stdout);
    clock_gettime(CLOCK_REALTIME, &start);
    taskCount = taskCountArg;
    taskIndex = 0;
    for (int i = 0; i < t; i++) {
        pthread_create(&pthread[i], NULL, functionPointer, NULL);
    }
    for (int i = 0; i < t; i++) {
        pthread_join(pthread[i], NULL);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    printf("%d ms\n", timeDifference(&start, &end));
}


void checkHammingResults() {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (distSerial[i][j] != distR[i][j]) {
                fprintf(stderr, "Row Parallelization Data Mismatch\n");
                fflush(stderr);
                exit(EXIT_FAILURE);
            }
            if (distSerial[i][j] != distC[i][j]) {
                fprintf(stderr, "Cell Parallelization Data Mismatch\n");
                fflush(stderr);
                exit(EXIT_FAILURE);
            }
            if (distSerial[i][j] != distCH[i][j]) {
                fprintf(stderr, "Character Parallelization Data Mismatch\n");
                fflush(stderr);
                exit(EXIT_FAILURE);
            }
        }
    }
}


int main(int argc, char **argv) {
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

    printf("\nArguments\n");
    printf("-------------------\n");
    printf("     m: %d\n", m);
    printf("     n: %d\n", n);
    printf("     l: %d\n", l);
    printf("     t: %d\n", t);

    initTables();

    printf("Calculation\n");
    serialHamming();
    threadSpawner("- Parallel by Row Execution: ", rowThread, (u_int64_t) n);
    threadSpawner("- Parallel by Cell Execution: ", cellThread, (u_int64_t) n * m);
    threadSpawner("- Parallel by Char Execution: ", charThread, (u_int64_t) n * m * l);

    checkHammingResults();
    printf("\n");
    return 0;
}