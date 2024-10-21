#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../src/shared/matrix.h"
#include "../src/cpu/matrix_singlethread.h"
#include "../src/shared/matrix_utils.h"

int main() {

    printf("%s\n", "--------STARTING matrix_mult_verification.c--------");

    // Benchmark parameters
    const int RUN_COUNT = 1;
    const int WARMUP_COUNT = 1;
    const int AVERAGE_COUNT = 1;
    const int BLOCK_SIZE = 1;

    // Matrix generation parameters
    const double VALUES_MIN = -1e-9;
    const double VALUES_MAX = 1e-9;
    const size_t DIMENSIONS_MIN = 1000;
    const size_t DIMENSIONS_MAX = 1000;
    const int seed = 42;

    struct timespec start, end;
    double elapsed_time;

    // Set the seed for reproducibility
    srand(seed);

    double total_time = 0;
    for (size_t i = 0; i < RUN_COUNT; i++) {

        // Generate Matrix dimensions
        const size_t n = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t m = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t p = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);

        // Generate matrices
        Matrix* A = generate_matrix(VALUES_MIN, VALUES_MAX, n, m);
        Matrix* B = generate_matrix(VALUES_MIN, VALUES_MAX, m, p);

        // openBLAS requires the resulting C array as well as argument
        double* C_blas = (double*)malloc(sizeof(double) * n * p);

        // Perform warmup
        for (size_t j = 0; j < WARMUP_COUNT; j++) {
            // Do the matrix multiplications
            Matrix* C = matrix_singlethread_mult(A, B, 1);
            matrix_mult_openblas(A->values, B->values, C_blas, n, m, p);

            matrix_free(C);
        }

        double time = 0;
        for (size_t j = 0; j < AVERAGE_COUNT; j++) {

            clock_gettime(CLOCK_MONOTONIC, &start);
            // Do the matrix multiplications
            Matrix* C = matrix_singlethread_mult(A, B, 1);
            matrix_mult_openblas(A->values, B->values, C_blas, n, m, p);
            clock_gettime(CLOCK_MONOTONIC, &end);

            matrix_free(C);

            // Calculate elapsed time in seconds
            elapsed_time = (end.tv_sec - start.tv_sec) +
                           (end.tv_nsec - start.tv_nsec) / 1e9;
            time += elapsed_time;
        }

        total_time += time / AVERAGE_COUNT;

        // Free the allocated data corresponding to this run
        matrix_free(A);
        matrix_free(B);
        free(C_blas);
    }

    printf("%s\n", "--------FINISHED matrix_mult_verification.c--------");

    return 0;
}

