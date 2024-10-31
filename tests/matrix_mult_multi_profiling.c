#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../src/shared/matrix.h"
#include "../src/cpu/matrix_multithread.h"
#include "../src/shared/matrix_utils.h"

int main() {

    printf("%s\n", "--------STARTING matrix_mult_multi_profiling.c--------");

    // Benchmark parameters
    const size_t BLOCK_SIZE = 128;
    const double APPROXIMATION_THRESHOLD = 1e-9;
    const size_t NUM_THREADS = 16;

    // Matrix generation parameters
    const double VALUES_MIN = -1e-9;
    const double VALUES_MAX = 1e-9;
    const size_t DIMENSIONS_MIN = 3000;
    const size_t DIMENSIONS_MAX = 3000;
    const int seed = 100;

    // Set the seed for reproducibility
    srand(seed);

    // Utils for tracking time
    struct timespec start, end;
    double elapsed_time_multi = 0;
    double elapsed_time_single = 0;

    double total_time = 0;

    printf("\n");

    // Generate Matrix dimensions
    const size_t n = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
    const size_t m = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
    const size_t p = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);

    // Generate matrices
    Matrix* A = generate_matrix(VALUES_MIN, VALUES_MAX, n, m);
    Matrix* B = generate_matrix(VALUES_MIN, VALUES_MAX, m, p);

    // Do the matrix multiplications
    Matrix* C = NULL;

    // Start timer
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Do multithread multiplication
    //C = matrix_singlethread_mult(A, B, BLOCK_SIZE);
    C = matrix_multithread_mult(A, B, BLOCK_SIZE, NUM_THREADS);

    // End timer
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate elapsed time in seconds
    elapsed_time_single = (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%-35s %f\n", "MULTITHREAD Elapsed time:", elapsed_time_single);

    // Free the allocated data corresponding to this run
    matrix_free(A);
    matrix_free(B);
    matrix_free(C);

    printf("%s\n", "--------FINISHED matrix_mult_multi_profiling.c--------");

    return 0;
}

