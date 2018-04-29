#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

double gettime(void) {
    struct timeval ttime;
    gettimeofday(&ttime, NULL);
    return ttime.tv_sec + ttime.tv_usec * 0.000001;
}

float randpval() {
    int vr = rand();
    int vm = rand() % vr;
    float r = ((float) vm) / (float) vr;
    assert(r >= 0.0 && r <= 1.00001);
    return r;
}

int main(int argc, char **argv) {
    //  Check arguments
    if (argc != 2) {
        printf("Invalid Arguments\n");
        return -1;
    }

    int N = atoi(argv[1]);
    int iters = 1000;
    srand(1);
    float *mVec = (float *) malloc(sizeof(float) * N);
    assert(mVec != NULL);
    float *nVec = (float *) malloc(sizeof(float) * N);
    assert(nVec != NULL);
    float *LVec = (float *) malloc(sizeof(float) * N);
    assert(LVec != NULL);
    float *RVec = (float *) malloc(sizeof(float) * N);
    assert(RVec != NULL);
    float *CVec = (float *) malloc(sizeof(float) * N);
    assert(CVec != NULL);
    float *FVec = (float *) malloc(sizeof(float) * N);
    assert(FVec != NULL);
    for (int i = 0; i < N; i++) {
        mVec[i] = (float) (2 + rand() % 10);
        nVec[i] = (float) (2 + rand() % 10);
        LVec[i] = 0.0;
        for (int j = 0; j < mVec[i]; j++) {
            LVec[i] += randpval();
        }
        RVec[i] = 0.0;
        for (int j = 0; j < nVec[i]; j++) {
            RVec[i] += randpval();
        }
        CVec[i] = 0.0;
        for (int j = 0; j < mVec[i] * nVec[i]; j++) {
            CVec[i] += randpval();
        }
        FVec[i] = 0.0;
        assert(mVec[i] >= 2.0 && mVec[i] <= 12.0);
        assert(nVec[i] >= 2.0 && nVec[i] <= 12.0);
        assert(LVec[i] > 0.0 && LVec[i] <= 1.0 * mVec[i]);
        assert(RVec[i] > 0.0 && RVec[i] <= 1.0 * nVec[i]);
        assert(CVec[i] > 0.0 && CVec[i] <= 1.0 * mVec[i] * nVec[i]);
    }
    float maxF = 0.0f;
    double timeTotal = 0.0f;
    for (int j = 0; j < iters; j++) {
        double time0 = gettime();
        for (int i = 0; i < N; i++) {
            float num_0 = LVec[i] + RVec[i];
            float num_1 = mVec[i] * (mVec[i] - 1.0) / 2.0;
            float num_2 = nVec[i] * (nVec[i] - 1.0) / 2.0;
            float num = num_0 / (num_1 + num_2);
            float den_0 = CVec[i] - LVec[i] - RVec[i];
            float den_1 = mVec[i] * nVec[i];
            float den = den_0 / den_1;
            FVec[i] = num / (den + 0.01);
            maxF = FVec[i] > maxF ? FVec[i] : maxF;
        }
        double time1 = gettime();
        timeTotal += time1 - time0;
    }
    printf("Time %f Max %f\n", timeTotal / iters, maxF);
    free(mVec);
    free(nVec);
    free(LVec);
    free(RVec);
    free(CVec);
    free(FVec);
}