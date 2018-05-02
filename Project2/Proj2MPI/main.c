#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <xmmintrin.h>
#include <mpi.h>

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


    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


    int N = atoi(argv[1]);
    int k = N % 4;
    int iters = 1000;
    float maxF = 0.0f;
    srand(1);

    float *mVec = (float *) _mm_malloc(sizeof(float) * N, 16);
    assert(mVec != NULL);
    float *nVec = (float *) _mm_malloc(sizeof(float) * N, 16);
    assert(nVec != NULL);
    float *LVec = (float *) _mm_malloc(sizeof(float) * N, 16);
    assert(LVec != NULL);
    float *RVec = (float *) _mm_malloc(sizeof(float) * N, 16);
    assert(RVec != NULL);
    float *CVec = (float *) _mm_malloc(sizeof(float) * N, 16);
    assert(CVec != NULL);
    float *FVec = (float *) _mm_malloc(sizeof(float) * N, 16);
    assert(FVec != NULL);
    float *maxv = (float *) _mm_malloc(sizeof(float), 16);
    assert(maxv != NULL);

    for (int i = 0; i < 4; i++) maxv[i] = -1;

    for (int i = 0; i < N; i++) {
        mVec[i] = (float) (2 + rand() % 10); // same as mVec[i/4].f[i%4]
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


    __m128 *mVec__m128 = (__m128 *) mVec;
    __m128 *nVec__m128 = (__m128 *) nVec;
    __m128 *LVec__m128 = (__m128 *) LVec;
    __m128 *RVec__m128 = (__m128 *) RVec;
    __m128 *CVec__m128 = (__m128 *) CVec;
    __m128 *FVec__m128 = (__m128 *) FVec;
    __m128 *maxv__m128 = (__m128 *) maxv;

    __m128 vec_num;
    __m128 vec_num_0;
    __m128 vec_num_1;
    __m128 vec_num_2;
    __m128 vec_den;
    __m128 vec_den_0;
    __m128 vec_den_1;

    const __m128 onev = _mm_set1_ps(1.f);
    const __m128 pponev = _mm_set1_ps(0.01f);
    const __m128 twov = _mm_set1_ps(2.f);


    int processSize = (N/4) / world_size;
    int start = processSize * world_rank;
    int end = start + processSize;

    double timeTotal = 0.0f;
    for (int j = 0; j < iters; j++) {
        double time0 = gettime();
        //Set MAX to 0
        maxF = 0.0f;

        for (int i = start; i < end; i++) {
            //float num_0 = LVec[i] + RVec[i];
            vec_num_0 = _mm_add_ps(LVec__m128[i], RVec__m128[i]);

            //float num_1 = mVec[i] * (mVec[i] - 1.0) / 2.0;
            vec_num_1 = _mm_sub_ps(mVec__m128[i], onev);
            vec_num_1 = _mm_div_ps(vec_num_1, twov);
            vec_num_1 = _mm_mul_ps(vec_num_1, mVec__m128[i]);

            //float num_2 = nVec[i] * (nVec[i] - 1.0) / 2.0;
            vec_num_2 = _mm_sub_ps(nVec__m128[i], onev);
            vec_num_2 = _mm_div_ps(vec_num_2, twov);
            vec_num_2 = _mm_mul_ps(vec_num_2, nVec__m128[i]);

            //float num = num_0 / (num_1 + num_2);
            vec_num = _mm_add_ps(vec_num_1, vec_num_2);
            vec_num = _mm_div_ps(vec_num_0, vec_num);

            //float den_0 = CVec[i] - LVec[i] - RVec[i];
            vec_den_0 = _mm_sub_ps(CVec__m128[i], LVec__m128[i]);
            vec_den_0 = _mm_sub_ps(vec_den_0, RVec__m128[i]);

            //float den_1 = mVec[i] * nVec[i];
            vec_den_1 = _mm_mul_ps(mVec__m128[i], nVec__m128[i]);

            //float den = den_0 / den_1;
            vec_den = _mm_div_ps(vec_den_0, vec_den_1);

            //FVec[i] = num / (den + 0.01);
            FVec__m128[i] = _mm_add_ps(vec_den, pponev);
            FVec__m128[i] = _mm_div_ps(vec_num, FVec__m128[i]);

            //maxF = FVec[i]>maxF?FVec[i]:maxF;
            *maxv__m128 = _mm_max_ps(*maxv__m128, FVec__m128[i]);
        }

        maxF = maxv[0];
        maxF = maxv[1] > maxF ? maxv[1] : maxF;
        maxF = maxv[2] > maxF ? maxv[2] : maxF;
        maxF = maxv[3] > maxF ? maxv[3] : maxF;

        if (!world_rank) {
            //use simple SSE way to compute remaining of array that is divisible with 4 ( (N/4)-processSize*world_size iterations )
            start = processSize*world_size;
            end = N/4;
            for (int i = start; i < end; i++) {
                //float num_0 = LVec[i] + RVec[i];
                vec_num_0 = _mm_add_ps(LVec__m128[i], RVec__m128[i]);

                //float num_1 = mVec[i] * (mVec[i] - 1.0) / 2.0;
                vec_num_1 = _mm_sub_ps(mVec__m128[i], onev);
                vec_num_1 = _mm_div_ps(vec_num_1, twov);
                vec_num_1 = _mm_mul_ps(vec_num_1, mVec__m128[i]);

                //float num_2 = nVec[i] * (nVec[i] - 1.0) / 2.0;
                vec_num_2 = _mm_sub_ps(nVec__m128[i], onev);
                vec_num_2 = _mm_div_ps(vec_num_2, twov);
                vec_num_2 = _mm_mul_ps(vec_num_2, nVec__m128[i]);

                //float num = num_0 / (num_1 + num_2);
                vec_num = _mm_add_ps(vec_num_1, vec_num_2);
                vec_num = _mm_div_ps(vec_num_0, vec_num);

                //float den_0 = CVec[i] - LVec[i] - RVec[i];
                vec_den_0 = _mm_sub_ps(CVec__m128[i], LVec__m128[i]);
                vec_den_0 = _mm_sub_ps(vec_den_0, RVec__m128[i]);

                //float den_1 = mVec[i] * nVec[i];
                vec_den_1 = _mm_mul_ps(mVec__m128[i], nVec__m128[i]);

                //float den = den_0 / den_1;
                vec_den = _mm_div_ps(vec_den_0, vec_den_1);

                //FVec[i] = num / (den + 0.01);
                FVec__m128[i] = _mm_add_ps(vec_den, pponev);
                FVec__m128[i] = _mm_div_ps(vec_num, FVec__m128[i]);

                //maxF = FVec[i]>maxF?FVec[i]:maxF;
                *maxv__m128 = _mm_max_ps(*maxv__m128, FVec__m128[i]);
            }
            start = N-k;
            end = N;
            for (int i = start; i < end; i++) {
                //use scalar (traditional) way to compute remaining of array ( N%4 iterations )
                float num_0 = LVec[i] + RVec[i];
                float num_1 = mVec[i] * (mVec[i] - 1.f) / 2.f;
                float num_2 = nVec[i] * (nVec[i] - 1.f) / 2.f;
                float num = num_0 / (num_1 + num_2);
                float den_0 = CVec[i] - LVec[i] - RVec[i];
                float den_1 = mVec[i] * nVec[i];
                float den = den_0 / den_1;
                FVec[i] = num / (den + 0.01f);
                maxF = FVec[i] > maxF ? FVec[i] : maxF;
            }
        }
        double time1 = gettime();
        timeTotal += time1 - time0;
    }

    float *processMax = world_rank ? NULL : (float *) malloc(sizeof(float) * world_size);
    MPI_Gather(&maxF, 1, MPI_FLOAT, processMax, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Finalize the MPI environment.
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    if (!world_rank) {
        float globalMax = 0;
        for (int i = 0; i < world_size; i++) {
            globalMax = globalMax < processMax[i] ? processMax[i] : globalMax;
        }
		printf("Time: %f us, Max %f\n", (timeTotal / iters)*1000000, globalMax);
        // printf("Time %f Max %f\n", timeTotal / iters, globalMax);
    }
    free(processMax);

    _mm_free(mVec);
    _mm_free(nVec);
    _mm_free(LVec);
    _mm_free(RVec);
    _mm_free(CVec);
    return 0;
}