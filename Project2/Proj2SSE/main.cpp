#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <xmmintrin.h>

struct Vec4
{
    union {
        __m128 v;
        float f[4];
    };
};

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

    // works serialized with mm_malloc


    struct Vec4* mVec;
    mVec = (Vec4*)_mm_malloc(sizeof(Vec4)*N, 16);
    struct Vec4* nVec;
    nVec = (Vec4*)_mm_malloc(sizeof(Vec4)*N, 16);
    struct Vec4* LVec;
    LVec = (Vec4*)_mm_malloc(sizeof(Vec4)*N, 16);
    struct Vec4* RVec;
    RVec = (Vec4*)_mm_malloc(sizeof(Vec4)*N, 16);
    struct Vec4* CVec;
    CVec = (Vec4*)_mm_malloc(sizeof(Vec4)*N, 16);
    struct Vec4* FVec;
    FVec = (Vec4*)_mm_malloc(sizeof(Vec4)*N, 16);

    assert(mVec != NULL);
    assert(nVec != NULL);
    assert(LVec != NULL);
    assert(RVec != NULL);
    assert(CVec != NULL);
    assert(FVec != NULL);


    for (int i = 0; i < N; i++) {
        mVec->f[i] = (float) (2 + rand() % 10); // same as mVec[i/4].f[i%4]
        nVec->f[i] = (float) (2 + rand() % 10);
        LVec->f[i] = 0.0;
        for (int j = 0; j < mVec->f[i]; j++) {
            LVec->f[i] += randpval();
        }
        RVec->f[i] = 0.0;
        for (int j = 0; j < nVec->f[i]; j++) {
            RVec->f[i] += randpval();
        }
        CVec->f[i] = 0.0;
        for (int j = 0; j < mVec->f[i] * nVec->f[i]; j++) {
            CVec->f[i] += randpval();
        }
        FVec->f[i] = 0.0;
        assert(mVec->f[i] >= 2.0 && mVec->f[i] <= 12.0);
        assert(nVec->f[i] >= 2.0 && nVec->f[i] <= 12.0);
        assert(LVec->f[i] > 0.0 && LVec->f[i] <= 1.0 * mVec->f[i]);
        assert(RVec->f[i] > 0.0 && RVec->f[i] <= 1.0 * nVec->f[i]);
        assert(CVec->f[i] > 0.0 && CVec->f[i] <= 1.0 * mVec->f[i] * nVec->f[i]);
    }
    float maxF = 0.0f;
    double timeTotal = 0.0f;

    __m128 * vec_num = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_num_0 = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_num_1 = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_num_2 = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_den = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_den_0 = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_den_1 = (__m128*)_mm_malloc(sizeof(float)*4, 16);


    struct Vec4* onev;
    struct Vec4* twov;
    struct Vec4* pponev;

    onev = (Vec4*)_mm_malloc(sizeof(Vec4), 16);
    pponev = (Vec4*)_mm_malloc(sizeof(Vec4), 16);
    twov = (Vec4*)_mm_malloc(sizeof(Vec4), 16);

    onev->v = _mm_set1_ps(1.f);
    pponev->v = _mm_set1_ps(0.01);
    twov->v = _mm_set1_ps(2.f);

    for (int i = 0 ; i < 8 ;i ++){
        printf("%f\n", onev->f[i]);
    }

    for (int j = 0; j < iters; j++) {
        double time0 = gettime();
        for (int i = 0; i < N/4; i++) {

            //float num_0 = LVec[i] + RVec[i];
            *vec_num_0 = _mm_add_ps(LVec[i].v, RVec[i].v);

            //float num_1 = mVec[i] * (mVec[i] - 1.0) / 2.0;
            *vec_num_1 = _mm_sub_ps(mVec[i].v, onev->v);
            *vec_num_1 = _mm_div_ps(*vec_num_1,twov->v);
            *vec_num_1 = _mm_mul_ps(*vec_num_1,mVec[i].v);

            //float num_2 = nVec[i] * (nVec[i] - 1.0) / 2.0;
            *vec_num_2 = _mm_sub_ps(nVec[i].v, onev->v);
            *vec_num_2 = _mm_div_ps(*vec_num_2,twov->v);
            *vec_num_2 = _mm_mul_ps(*vec_num_2,nVec[i].v);

            //float num = num_0 / (num_1 + num_2);
            *vec_num = _mm_add_ps(*vec_num_1, *vec_num_2);
            *vec_num = _mm_div_ps(*vec_num_0,*vec_num);

            //float den_0 = CVec[i] - LVec[i] - RVec[i];
            *vec_den_0 = _mm_sub_ps(CVec[i].v, LVec[i].v);
            *vec_den_0 = _mm_sub_ps(*vec_den_0, RVec[i].v);

            //float den_1 = mVec[i] * nVec[i];
            *vec_den_1 = _mm_mul_ps(mVec[i].v ,nVec[i].v);

            //float den = den_0 / den_1;
            *vec_den = _mm_div_ps(*vec_den_0 ,*vec_den_1);

            //FVec[i] = num / (den + 0.01);
            FVec[i].v = _mm_add_ps(*vec_den, pponev->v);
            FVec[i].v = _mm_div_ps(*vec_num ,FVec[i].v);

        }

        //TODO vectorize the MAX algorithm
        for (int i = 0; i < N; i++) {
            maxF = FVec->f[i] > maxF ? FVec->f[i] : maxF;
        }
        //TODO use scalar (traditional) way to compute remainigs of array ( N%4 iterations )

//            float num_0 = LVec[i] + RVec[i];
//            float num_1 = mVec[i] * (mVec[i] - 1.0) / 2.0;
//            float num_2 = nVec[i] * (nVec[i] - 1.0) / 2.0;
//            float num = num_0 / (num_1 + num_2);
//            float den_0 = CVec[i] - LVec[i] - RVec[i];
//            float den_1 = mVec[i] * nVec[i];
//            float den = den_0 / den_1;
//            FVec[i] = num / (den + 0.01);

        double time1 = gettime();
        timeTotal += time1 - time0;
    }
    printf("Time %f Max %f\n", timeTotal / iters, maxF);
    _mm_free(mVec);
    _mm_free(nVec);
    _mm_free(LVec);
    _mm_free(RVec);
    _mm_free(CVec);
    _mm_free(vec_den);
    _mm_free(vec_den_0);
    _mm_free(vec_den_1);
    _mm_free(vec_num);
    _mm_free(vec_num_0);
    _mm_free(vec_num_1);
    _mm_free(vec_num_2);
}