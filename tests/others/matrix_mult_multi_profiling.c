/**
 * @file matrix_mult_multi_profiling.c
 *
 * @brief Contains the minimum needed to create matrices A and B
 * and perform Matrix multiplication. This reduces overhead and
 * makes the profiling report simpler to read.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../src/shared/matrix.h"
#include "../../src/cpu/matrix_multithread_9avx.h"
#include "../../src/shared/matrix_utils.h"

int main() {

    // Benchmark parameters
    const size_t BLOCK_SIZE = 128;
    const double APPROXIMATION_THRESHOLD = 1e-9;
    const size_t NUM_THREADS = 16;

    // Matrix generation parameters
    const double VALUES_MIN = -1e+6;
    const double VALUES_MAX = 1e+6;
    const size_t DIMENSIONS_MIN = 750;
    const size_t DIMENSIONS_MAX = 750;
    const int seed = 100;

    // Set the seed for reproducibility
    srand(seed);

    // Generate Matrix dimensions
    const size_t n = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
    const size_t m = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
    const size_t p = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);

    // Generate matrices
    Matrix* A = generate_matrix(VALUES_MIN, VALUES_MAX, n, m);
    Matrix* B = generate_matrix(VALUES_MIN, VALUES_MAX, m, p);

    // Allocate C Matrix
    Matrix* C = matrix_create_with(pattern_zero, NULL, n, p);

    // Do multithread multiplication
    matrix_multithread_mult_9avx(A, B, C, BLOCK_SIZE, NUM_THREADS);

    // Free the allocated data corresponding to this run
    matrix_free(A);
    matrix_free(B);
    matrix_free(C);

    return 0;
}

