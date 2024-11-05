#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../../src/shared/matrix.h"
#include "../../src/cpu/matrix_mult_naive.h"
#include "../../src/cpu/matrix_multithread.h"
#include "../../src/cpu/matrix_singlethread.h"
#include "../../src/shared/matrix_utils.h"

// Algorithms that will be tested
typedef enum {
    BLAS,
    NAIVE,
    SINGLETHREAD,
    MULTITHREAD,
    MULTITHREAD_9AVX
} Algorithm;

void warm_up(const size_t WARM_UP_COUNT) {


}


Matrix* run_algorithm(Algorithm algo, Matrix* A, Matrix* B, double* C_blas,
                      const size_t BLOCK_SIZE, const size_t NUM_THREADS,
                      const size_t n, const size_t m, const size_t p) {

    switch (algo) {
        case BLAS:
            matrix_mult_openblas(A->values, B->values, C_blas, n, m, p);
            break;
        case NAIVE:
            matrix_mult_naive(A, B);
            break;
        case SINGLETHREAD:
            matrix_singlethread_mult(A, B, BLOCK_SIZE);
            break;
        case MULTITHREAD:
            matrix_multithread_mult(A, B, BLOCK_SIZE, NUM_THREADS);
            break;
        case MULTITHREAD_9AVX:
            // TODO separate multithread and _9AVX somehow
            matrix_multithread_mult(A, B, BLOCK_SIZE, NUM_THREADS);
            break;
    }

    return NULL;
}


int main() {

    printf("%s\n", "--------STARTING matrix_mult_benchmark.c--------");

    // Benchmark parameters
    const size_t RUN_COUNT = 10;
    const size_t WARM_UP_COUNT = 10;
    const size_t BLOCK_SIZE = 128;
    // Used if there are different rounding errors between the implementations
    const double APPROXIMATION_THRESHOLD = 1e-9;
    const size_t NUM_THREADS = 16;

    // Matrix generation parameters
    const double VALUES_MIN = -1e+6;
    const double VALUES_MAX = 1e+6;
    const size_t DIMENSIONS_MIN = 3000;
    const size_t DIMENSIONS_MAX = 3000;
    const int seed = 42;
    const bool compare_to_BLAS = false;

    // Set the seed for reproducibility
    srand(seed);

    // Utils for tracking time
    struct timespec start, end;
    double elapsed_time_multi = 0;
    double elapsed_time_single = 0;

    // Perform the warm-up
    warm_up(WARM_UP_COUNT);

    bool mismatch_detected = true;
    double total_time = 0;
    for (size_t i = 0; i < RUN_COUNT; i++) {

        printf("%s %zu\n", "Iteration", i);

        // Generate Matrix dimensions
        const size_t n = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t m = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t p = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);

        // Generate matrices
        Matrix* A = generate_matrix(VALUES_MIN, VALUES_MAX, n, m);
        Matrix* B = generate_matrix(VALUES_MIN, VALUES_MAX, m, p);

        // openBLAS requires the resulting C array as well as argument
        double* C_blas = (double*)malloc(sizeof(double) * n * p);

        // Do the matrix multiplications
        Matrix* C = NULL;

        // Start timer
        clock_gettime(CLOCK_MONOTONIC, &start);

        // Perform multiplication

        // End timer
        clock_gettime(CLOCK_MONOTONIC, &end);

        // Calculate elapsed time in seconds
        elapsed_time_multi = (end.tv_sec - start.tv_sec) +
            (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("%-35s %f\n", "MULTI Elapsed time:", elapsed_time_multi);

        // Compare result
        for (size_t j = 0; j < n * p; j++) {

            if (fabs(C->values[j] - C_blas[j]) > APPROXIMATION_THRESHOLD) {
                printf("Error: The matrix mult result differs!\n");
                mismatch_detected = true;
                printf("My implementation %f\n", C->values[j]);
                printf("BLAS implementation %f\n", C_blas[j]);

                matrix_free(A);
                matrix_free(B);
                matrix_free(C);
                free(C_blas);

                return 0;
            }
        }

        Matrix* C_m = matrix_create_with(pattern_zero, NULL, n, p);
        C_m->values = C_blas;

        // Free the allocated data corresponding to this run
        matrix_free(A);
        matrix_free(B);
        matrix_free(C);
        free(C_blas);
    }

    printf("%s\n", "All calculations are correct");
    printf("%s\n", "--------FINISHED matrix_mult_benchmark.c--------");

    return 0;
}

