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
    //  Check arguments
    if (argc != 2) {
        printf("Invalid Arguments\n");
        return -1;
    }

    int N = atoi(argv[1]);
    int iters = 1000;
    srand(1);

    // works serialized with mm_malloc
    float *mVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    __m128 * mVec_ptr = (__m128*) mVec;
    assert(mVec != NULL);
    float *nVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    __m128 * nVec_ptr = (__m128*) nVec;
    assert(nVec != NULL);
    float *LVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    __m128 * LVec_ptr = (__m128*) LVec;
    assert(LVec != NULL);
    float *RVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    __m128 * RVec_ptr = (__m128*) RVec;
    assert(RVec != NULL);
    float *CVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    __m128 * CVec_ptr = (__m128*) CVec;
    assert(CVec != NULL);
    float *FVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    __m128 * FVec_ptr = (__m128*) FVec;
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

    __m128 * vec_num = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_num_0 = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_num_1 = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_num_2 = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_den = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_den_0 = (__m128*)_mm_malloc(sizeof(float)*4, 16);
    __m128 * vec_den_1 = (__m128*)_mm_malloc(sizeof(float)*4, 16);

    float *onesVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    __m128 * onesVec_ptr = (__m128*) CVec;
    assert(onesVec != NULL);
    float *twosVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    __m128 * twosVec_ptr = (__m128*) FVec;
    assert(twosVec != NULL);
    float *pponesVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    __m128 * pponesVec_ptr = (__m128*) FVec;
    assert(pponesVec != NULL);

    for (int i = 0 ; i < 4 ; i++){
        onesVec[i] = 1.0;
        pponesVec[i] = 0.01;
        twosVec[i] = 2.0;
    }

    for (int j = 0; j < iters; j++) {
        double time0 = gettime();


        //To be SIMDed
        for (int i = 0; i < N/4; i++) {

//            float num_0 = LVec[i] + RVec[i];
//            float num_1 = mVec[i] * (mVec[i] - 1.0) / 2.0;
//            float num_2 = nVec[i] * (nVec[i] - 1.0) / 2.0;
//            float num = num_0 / (num_1 + num_2);
//            float den_0 = CVec[i] - LVec[i] - RVec[i];
//            float den_1 = mVec[i] * nVec[i];
//            float den = den_0 / den_1;
//            FVec[i] = num / (den + 0.01);

            //float num_0 = LVec[i] + RVec[i];
            *vec_num_0 = _mm_add_ps(LVec_ptr[i], RVec_ptr[i]);

            //float num_1 = mVec[i] * (mVec[i] - 1.0) / 2.0;
            *vec_num_1 = _mm_sub_ps(mVec_ptr[i], *onesVec_ptr);
            *vec_num_1 = _mm_div_ps(*vec_num_1,*twosVec_ptr);
            *vec_num_1 = _mm_mul_ps(*vec_num_1,mVec_ptr[i]);

            //float num_2 = nVec[i] * (nVec[i] - 1.0) / 2.0;
            *vec_num_2 = _mm_sub_ps(nVec_ptr[i], *onesVec_ptr);
            *vec_num_2 = _mm_div_ps(*vec_num_2,*twosVec_ptr);
            *vec_num_2 = _mm_mul_ps(*vec_num_2,nVec_ptr[i]);


            //float num = num_0 / (num_1 + num_2);
            *vec_num = _mm_add_ps(*vec_num_1, *vec_num_2);
            *vec_num = _mm_div_ps(*vec_num_0,*vec_num);

            //float den_0 = CVec[i] - LVec[i] - RVec[i];
            *vec_den_0 = _mm_sub_ps(CVec_ptr[i], LVec_ptr[i]);
            *vec_den_0 = _mm_sub_ps(*vec_den_0, RVec_ptr[i]);


            //float den_1 = mVec[i] * nVec[i];
            *vec_den_1 = _mm_mul_ps(mVec_ptr[i] ,nVec_ptr[i]);

            //float den = den_0 / den_1;
            *vec_den = _mm_div_ps(*vec_den_0 ,*vec_den_1);

            //FVec[i] = num / (den + 0.01);
            FVec_ptr[i] = _mm_add_ps(*twosVec_ptr, *twosVec_ptr);
            //FVec_ptr[i] = _mm_div_ps(*vec_num ,FVec_ptr[i]);

        }
        for (int i = 0; i < N; i++) {
            maxF = FVec[i] > maxF ? FVec[i] : maxF;
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
    _mm_free(FVec);
}