#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


void *rowThread(void *threadarg);
void *cellThread(void *threadarg);
void *charThread(void *threadarg);
int hamming(int l , int* a, int* b);

struct rowThreadData{
    int  tid;
    int  n;
    int  row;
    int  l;
    int** a;
    int** b;
    int** distR;
};

struct cellThreadData{
    int  tid;
    int  row;
    int  cell;
    int  l;
    int** a;
    int** b;
    int** distC;
};

struct charThreadData{
    int  tid;
    int  row;
    int  cell;
    int  l;
    int** a;
    int** b;
    int** distCH;
};

int main(int argc, char **argv) {
    int m,n,l, t ,i,j ;             // m = A strings, n = B strings , l = string length
    int **a, **b, **distSerial , **distR , **distC, **distCH ;    // Tables
    srand(time(NULL));          // init rand

    //Thread data
    struct rowThreadData *rowTD;
    struct cellThreadData *cellTD;
    struct charThreadData *charTD;

    //Threads
    pthread_t *rowT;
    pthread_t *cellT;
    pthread_t *charT;


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

//  Init Tables
    distSerial = malloc(m*sizeof(int*));
    distC = malloc(m*sizeof(int*));
    distR = malloc(m*sizeof(int*));
    distCH = malloc(m*sizeof(int*));
    rowTD = malloc(m*sizeof(struct rowThreadData*));
    cellTD = malloc(m*n*sizeof(struct cellThreadData*));
    charTD = malloc(m*n*l*sizeof(struct charThreadData*));
    rowT = malloc(m*sizeof(struct pthread_t*));
    cellT = malloc(m*n*sizeof(struct pthread_t*));
    charT = malloc(m*n*l*sizeof(struct pthread_t*));

    for (i=0;i<m;i++){
        distSerial[i]=malloc(n*sizeof(int));
        distR[i]=malloc(n*sizeof(int));
        distC[i]=malloc(n*sizeof(int));
        distCH[i]=malloc(n*sizeof(int));
    }

    for (i=0 ; i<m; i++){
        for (j=0;j<n;j++){
            distR[i][j] = 0;
            distC[i][j] = 0;
            distCH[i][j] = 0;
        }
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


//  Calculate Serial Hamming
    for (i=0 ; i<m; i++){
        for (j=0;j<n;j++){
            distSerial[i][j] = hamming(l,a[i],b[j]);
        }
    }

//  Calculate Serial Hamming, Parallelize each row
    for (i=0 ; i<m; i++){
        rowTD[i].tid = i ;
        rowTD[i].row = i ;
        rowTD[i].a = a ;
        rowTD[i].b = b ;
        rowTD[i].distR = distR;
        rowTD[i].l = l;
        rowTD[i].n = n;
        pthread_create(&rowT[i], NULL, rowThread, (void *) &rowTD[i]);

    }

    return 0;
}



void *rowThread(void *threadarg){
    int **dist;
    int i ,j ,k, l, n ,row, h=0;
    struct rowThreadData *args;
    int **a, **b;

    args = (struct rowThreadData *) threadarg;
    dist = args->distR;
    a = args->a;
    b = args->b;
    l = args->l;
    n = args->n;
    row = args->row;



    for(i=0;i<n;i++){
        //dist[args->row][i] = hamming(10,a[args->row],b[i]);
        k = 0;
        for ( j=0;j<50;j++){
//            if (a[row][j] == b[i][j]){
//                k=1;
//            }
        }
        dist[row][i] = k;
    }

    pthread_exit(NULL);  
}

void *cellThread(void *threadarg){

}

void *charThread(void *threadarg){

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