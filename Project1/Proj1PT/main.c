#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


void *rowThread(void *threadarg);

void *cellThread(void *threadarg);

void *charThread(void *threadarg);

int hamming(int *a, int *b);

int timedif(struct timespec *start, struct timespec *stop) {
    return ((int) ((stop->tv_sec - start->tv_sec) * 1000) + (int) ((stop->tv_nsec - start->tv_nsec) / 1000000));
}


struct rowThreadData {
    int tid;
    int n;
    int row;
    int **distR;
};

struct cellThreadData {
    int tid;
    int row;
    int cell;
    int **distC;
};

struct charThreadData {
    int tid;
    int row;
    int cell;
    int **distCH;
};
int **a, **b, l, m, n, t;
int **distSerial, **distR, **distC, **distCH;    // Tables

//Thread data
struct rowThreadData *rowTD;
struct cellThreadData *cellTD;
struct charThreadData *charTD;

//Threads
pthread_t *rowT;
pthread_t *cellT;
pthread_t *charT;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t counterMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond;
int threadCounter = 0;

struct timespec start, end;

void initTables() {
    int i, j;
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

    for (i = 0; i < m; i++) {
        distSerial[i] = malloc(n * sizeof(int));
        distR[i] = malloc(n * sizeof(int));
        distC[i] = malloc(n * sizeof(int));
        distCH[i] = malloc(n * sizeof(int));
    }

    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            distR[i][j] = 0;
            distC[i][j] = 0;
            distCH[i][j] = 0;
        }
    }

    a = malloc(m * sizeof(int *));
    for (i = 0; i < m; i++) {
        a[i] = malloc(l * sizeof(int));
    }

    b = malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        b[i] = malloc(l * sizeof(int));
    }

//  init A set
    for (i = 0; i < m; i++) {
        for (j = 0; j < l; j++) {
            a[i][j] = rand() % 2; // random number between 0 and 1
        }
    }

//  init B set
    for (i = 0; i < n; i++) {
        for (j = 0; j < l; j++) {
            b[i][j] = rand() % 2; // random number between 0 and 1
        }
    }
}
void serialHamming(){
    int i,j;
    clock_gettime(CLOCK_REALTIME,&start);
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            distSerial[i][j] = hamming(a[i], b[j]);
        }
    }
    clock_gettime(CLOCK_REALTIME,&end);
    printf("Serial: %d ms\n", timedif(&start,&end));
}

int main(int argc, char **argv) {
    int i, j;             // m = A strings, n = B strings , l = string length
    srand(time(NULL));          // init rand

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

    printf("\nArguments");
    printf("\n-------------------");
    printf("\n     m: %d", m);
    printf("\n     n: %d", n);
    printf("\n     l: %d", l);
    printf("\n     t: %d\n", t);

    initTables();
    printf("\n\nCalculation\n");
//  Calculate Serial Hamming
//    serialHamming();


    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&counterMutex, NULL);
    pthread_cond_init(&cond, 0);

//  Calculate Hamming, Parallelize each row
    clock_gettime(CLOCK_REALTIME, &start);
    for (i = 0; i < m; i++) {
//        if((i/m)*100%10==0){
//            printf(".");
//            fflush(stdout);
//        }
        rowTD[i].tid = i;
        rowTD[i].row = i;
        rowTD[i].distR = distR;
        rowTD[i].n = n;
        while(threadCounter==t){
            printf("Cond Wait %d\n", threadCounter);
            pthread_cond_wait(&cond, &mutex);
            printf("Cond awakened %d\n", threadCounter);
        }
//        printf("Cond awakened %d\n", threadCounter);
        pthread_mutex_lock(&counterMutex);
        threadCounter++;
        pthread_mutex_unlock(&counterMutex);
        pthread_create(&rowT[i], NULL, rowThread, (void *) &rowTD[i]);
    }
    for (i = 0; i < m; i++) {
        printf("Joining Thread: %d\n", i);
        pthread_join(rowT[i], NULL);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    printf("\nRow: %d ms", timedif(&start, &end));


    return 0;
}


void *rowThread(void *threadarg) {
    int **dist;
    int i, n, row;
    struct rowThreadData *args;
    args = (struct rowThreadData *) threadarg;
    dist = distR;
    n = args->n;
    row = args->row;
    if (a[row] == NULL) {
        printf("Damn");
    }
    for (i = 0; i < n; i++) {
        dist[row][i] = hamming(a[row], b[i]);
    }
    pthread_mutex_lock(&counterMutex);
    threadCounter--;
    pthread_mutex_unlock(&counterMutex);

    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&cond);
    printf("Cond broadcast %d\n", threadCounter);
}

void *cellThread(void *threadarg) {

}

void *charThread(void *threadarg) {

}

//Hamming Distance Calculator between two arrays of l size
int hamming(int *a, int *b) {
    int k = 0;
    for (int i = 0; i < l; i++) {
        if (a[i] != b[i]) {
            k++;
        }
    }
    return k;
}