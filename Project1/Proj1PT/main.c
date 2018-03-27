#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//Thread data
struct rowThreadData {
    int tid;
    int row;
} *rowTD;

struct cellThreadData {
    int tid;
    int row;
    int col;
} *cellTD;

struct charThreadData {
    int tid;
    int row;
    int col;
    int index;
} *charTD;
int **a, **b, m, n, l, t;
int **distSerial, **distR, **distC, **distCH;    // Tables

//Threads
pthread_t *rowT;
pthread_t *cellT;
pthread_t *charT;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t counterMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t **cellMutex;
pthread_cond_t cond;
int threadCounter = 0;

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
    rowTD = malloc(m * sizeof(struct rowThreadData *));
    cellTD = malloc(m * n * sizeof(struct cellThreadData *));
    charTD = malloc(m * n * l * sizeof(struct charThreadData *));
    rowT = malloc(m * sizeof(struct pthread_t *));
    cellT = malloc(m * n * sizeof(struct pthread_t *));
    charT = malloc(m * n * l * sizeof(struct pthread_t *));
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
    srand((unsigned int)time(NULL));
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


void *rowThread(void *threadarg) {
    int row;
    struct rowThreadData *args;
    args = (struct rowThreadData *) threadarg;
    row = args->row;
    for (int i = 0; i < n; i++) {
        distR[row][i] = hamming(a[row], b[i]);
    }
    pthread_mutex_lock(&counterMutex);
    threadCounter--;
    pthread_mutex_unlock(&counterMutex);
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&cond);
}


void rowParallelizedHamming() {
    printf("- Parallel by Row Execution: ");
    fflush(stdout);
    threadCounter = 0;
    clock_gettime(CLOCK_REALTIME, &start);
    for (int i = 0; i < m; i++) {
        rowTD[i].tid = i;
        rowTD[i].row = i;
        while (threadCounter == t) {
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_lock(&counterMutex);
        threadCounter++;
        pthread_mutex_unlock(&counterMutex);
        pthread_create(&rowT[i], NULL, rowThread, (void *) &rowTD[i]);
    }
    for (int i = 0; i < m; i++) {
        pthread_join(rowT[i], NULL);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    printf("%d ms\n", timeDifference(&start, &end));
}


void *cellThread(void *threadarg) {
    struct cellThreadData *args = threadarg;
    int res = hamming(a[args->row], b[args->col]);
    pthread_mutex_lock(&cellMutex[args->row][args->col]);
    distC[args->row][args->col] += res;
    pthread_mutex_unlock(&cellMutex[args->row][args->col]);
    pthread_mutex_lock(&counterMutex);
    threadCounter--;
    pthread_mutex_unlock(&counterMutex);
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&cond);
    printf("Wake %d\n", threadCounter);
}


void cellParallelizedHamming() {
    printf("- Parallel by Cell Execution: ");
    fflush(stdout);
    threadCounter = 0;
    int threadCounterTmp;
    u_int64_t index;
    clock_gettime(CLOCK_REALTIME, &start);
    for (uint i = 0; i < m; i++) {
        for (uint j = 0; j < n; j++) {
            index = i * j + j;
            cellTD[index].tid = i;
            cellTD[index].row = i;
            cellTD[index].col = j;
            pthread_mutex_lock(&counterMutex);
            threadCounterTmp = threadCounter;
            pthread_mutex_unlock(&counterMutex);
            while (threadCounterTmp == t) {
                printf("Sleep %d\n", threadCounter);
                pthread_cond_wait(&cond, &mutex);
                pthread_mutex_lock(&counterMutex);
                threadCounterTmp = threadCounter;
                pthread_mutex_unlock(&counterMutex);
            }
            pthread_mutex_lock(&counterMutex);
            threadCounter++;
            pthread_mutex_unlock(&counterMutex);
            pthread_create(&cellT[index], NULL, cellThread, (void *) &cellTD[index]);
            printf("Thread %d spawned\n", index);
        }
    }
    for (int i = 0; i < m * n; i++) {
        for (int j = 0; j < n; j++) {
            pthread_join(cellT[i * j + j], NULL);
        }
    }
    clock_gettime(CLOCK_REALTIME, &end);
    printf("%d ms\n", timeDifference(&start, &end));
}


void *charThread(void *threadarg) {
    struct charThreadData *args = threadarg;
    if (a[args->row][args->index] != b[args->col][args->index]) {
        pthread_mutex_lock(&cellMutex[args->row][args->col]);
        distCH[args->row][args->col]++;
        pthread_mutex_unlock(&cellMutex[args->row][args->col]);
    }
    pthread_mutex_lock(&counterMutex);
    threadCounter--;
    pthread_mutex_unlock(&counterMutex);
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&cond);
}


void charParallelizedHamming() {
    printf("- Parallel by Character Execution: ");
    fflush(stdout);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            pthread_mutex_unlock(&cellMutex[i][j]);
        }
    }
    threadCounter = 0;
    int index;
    clock_gettime(CLOCK_REALTIME, &start);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < l; k++) {
                index = i * j * k + j * k + k;
                charTD[index].tid = i;
                charTD[index].row = i;
                charTD[index].col = j;
                charTD[index].index = k;
                while (threadCounter == t) {
                    pthread_cond_wait(&cond, &mutex);
                }
                pthread_mutex_lock(&counterMutex);
                threadCounter++;
                pthread_mutex_unlock(&counterMutex);
                pthread_create(&charT[index], NULL, charThread, (void *) &charTD[index]);
            }
        }
    }
    for (u_int64_t i = 0; i < m * n * l; i++) {
        pthread_join(cellT[i], NULL);
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
//            if (distSerial[i][j] != distCH[i][j]){
//                fprintf(stderr, "Character Parallelization Data Mismatch\n");
//                fflush(stderr);
//                exit(EXIT_FAILURE);
//            }
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
    pthread_cond_init(&cond, 0);
    printf("Calculation\n");

    // Calculations
//    serialHamming();
//    rowParallelizedHamming();
    cellParallelizedHamming();
//    charParallelizedHamming();

    checkHammingResults();
    printf("\n");
    return 0;
}