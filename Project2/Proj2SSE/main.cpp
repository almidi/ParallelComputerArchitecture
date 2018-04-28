#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <xmmintrin.h>

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
    struct vec
    {
        union {
            __m128 v;
            float f[4];
        };
    };

    //  Check arguments
    if (argc != 2) {
        printf("Invalid Arguments\n");
        return -1;
    }

    int N = atoi(argv[1]);
    int k = N%4;
    int iters = 1000;
    float maxF = 0.0f;
    srand(1);

    struct vec* mVec;
    mVec = (vec*)_mm_malloc(sizeof(vec)*N, 16);
    assert(mVec != NULL);
    struct vec* nVec;
    nVec = (vec*)_mm_malloc(sizeof(vec)*N, 16);
    assert(nVec != NULL);
    struct vec* LVec;
    LVec = (vec*)_mm_malloc(sizeof(vec)*N, 16);
    assert(LVec != NULL);
    struct vec* RVec;
    RVec = (vec*)_mm_malloc(sizeof(vec)*N, 16);
    assert(RVec != NULL);
    struct vec* CVec;
    CVec = (vec*)_mm_malloc(sizeof(vec)*N, 16);
    assert(CVec != NULL);
    struct vec* FVec;
    FVec = (vec*)_mm_malloc(sizeof(vec)*N, 16);
    assert(FVec != NULL);
    struct vec* maxv;
    maxv = (vec*)_mm_malloc(sizeof(vec), 16);
    assert(maxv != NULL);

    __m128 * vec_num = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_num_0 = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_num_1 = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_num_2 = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_den = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_den_0 = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_den_1 = (__m128*)_mm_malloc(sizeof(float)*4, 16);

    __m128 * onev = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * pponev = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * twov = (__m128*)_mm_malloc(sizeof(float)*4, 16);

    *onev = _mm_set1_ps(1.f);
    *pponev = _mm_set1_ps(0.01f);
    *twov = _mm_set1_ps(2.f);

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

    double timeTotal = 0.0f;
    for (int j = 0; j < iters; j++) {
        double time0 = gettime();
        //Set MAX to 0
        maxv->v = _mm_set1_ps(0.f);
        maxF = 0.0f;

        for (int i = 0; i < N/4; i++) {
            //float num_0 = LVec[i] + RVec[i];
            *vec_num_0 = _mm_add_ps(LVec[i].v, RVec[i].v);

            //float num_1 = mVec[i] * (mVec[i] - 1.0) / 2.0;
            *vec_num_1 = _mm_sub_ps(mVec[i].v, *onev);
            *vec_num_1 = _mm_div_ps(*vec_num_1,*twov);
            *vec_num_1 = _mm_mul_ps(*vec_num_1,mVec[i].v);

            //float num_2 = nVec[i] * (nVec[i] - 1.0) / 2.0;
            *vec_num_2 = _mm_sub_ps(nVec[i].v, *onev);
            *vec_num_2 = _mm_div_ps(*vec_num_2,*twov);
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
            FVec[i].v = _mm_add_ps(*vec_den, *pponev);
            FVec[i].v = _mm_div_ps(*vec_num ,FVec[i].v);

            //maxF = FVec->f[j] > maxF ? FVec->f[j] : maxF;
            maxv->v = _mm_max_ps(maxv->v, FVec[i].v);
        }

        for(int j = 0 ; j < 4 ; j++){
            maxF = maxv->f[j] > maxF ? maxv->f[j] : maxF;
        }

        for(int j = N - k ; j < N ; j ++){
            // use scalar (traditional) way to compute remainigs of array ( N%4 iterations )
            float num_0 = LVec->f[j] + RVec->f[j];
            float num_1 = mVec->f[j] * (mVec->f[j] - 1.f) / 2.f;
            float num_2 = nVec->f[j] * (nVec->f[j] - 1.f) / 2.f;
            float num = num_0 / (num_1 + num_2);
            float den_0 = CVec->f[j] - LVec->f[j] - RVec->f[j];
            float den_1 = mVec->f[j] * nVec->f[j];
            float den = den_0 / den_1;
            FVec->f[j] = num / (den + 0.01f);
            maxF = FVec->f[j] > maxF ? FVec->f[j] : maxF;
        }
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
    _mm_free(maxv);
    _mm_free(onev);
    _mm_free(twov);
    _mm_free(pponev);
}