#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../src/shared/matrix.h"
#include "../../src/cpu/matrix_mult_naive.h"
#include "../../src/cpu/matrix_multithread.h"
#include "../../src/cpu/matrix_singlethread.h"
#include "../../src/shared/matrix_utils.h"

// Algorithms that will be tested
typedef enum {
    BLAS = 0, // = 0 to be able to loop through enums
    NAIVE,
    SINGLETHREAD,
    MULTITHREAD,
    MULTITHREAD_9AVX
} Algorithm;

/**
 * @brief Select the algorithm to use depending on the first
 * argument algo. If algo = BLAS, then the result will be
 * placed in the C_blas array. Else, the result will be placed
 * in the C Matrix.
 *
 * @param algo Enum for the algorithm to use.
 * @param A Pointer to the first Matrix (A x B = C).
 * @param B Pointer to the second Matrix (A x B = C).
 * @param C Pointer to the third Matrix (A x B = C) which is used
 * to place the result if algo != BLAS.
 * @param C_blas A pointer to a double array where the BLAS result
 * will be placed if algo = BLAS.
 * @param BLOCK_SIZE The block size to use for the blocking method
 * with the SINGLETHREAD, MULTITHREAD or MULTITHREAD_9AVX algorithm.
 * @param NUM_THREADS The number of threads to use with the
 * MULTITHREAD algorithm and MULTITHREAD_9AVX.
 * @param n The number of rows in A.
 * @param m The number of columns in A.
 * @param p The number of columns in B.
 */
void run_algorithm(Algorithm algo, Matrix* A, Matrix* B, Matrix* C,
                      double* C_blas, const size_t BLOCK_SIZE,
                      const size_t NUM_THREADS,
                      const size_t n, const size_t m, const size_t p) {

    switch (algo) {
        case BLAS:
            matrix_mult_openblas(A->values, B->values, C_blas, n, m, p);
            break;
        case NAIVE:
            matrix_mult_naive(A, B, C);
            break;
        case SINGLETHREAD:
            matrix_singlethread_mult(A, B, C, BLOCK_SIZE);
            break;
        case MULTITHREAD:
            matrix_multithread_mult(A, B, C, BLOCK_SIZE, NUM_THREADS);
            break;
        case MULTITHREAD_9AVX:
            // TODO separate multithread and _9AVX somehow
            matrix_multithread_mult(A, B, C, BLOCK_SIZE, NUM_THREADS);
            break;
    }
}

/**
 * @brief Select the algorithm to use depending on the first
 * argument algo. Perform warm-up runs.
 *
 * @param algo Enum for the algorithm to use.
 * @param A Pointer to the first Matrix (A x B = C).
 * @param B Pointer to the second Matrix (A x B = C).
 * @param C Pointer to the third Matrix (A x B = C) which is used
 * to place the result if algo != BLAS.
 * @param C_blas A pointer to a double array where the BLAS result
 * will be placed if algo = BLAS.
 * @param BLOCK_SIZE The block size to use for the blocking method
 * with the SINGLETHREAD, MULTITHREAD or MULTITHREAD_9AVX algorithm.
 * @param NUM_THREADS The number of threads to use with the
 * MULTITHREAD algorithm and MULTITHREAD_9AVX.
 * @param n The number of rows in A.
 * @param m The number of columns in A.
 * @param p The number of columns in B.
 */
int warm_up(size_t WARM_UP_COUNT, Algorithm algo,
             const size_t DIMENSIONS_MIN, const size_t DIMENSIONS_MAX,
             const size_t VALUES_MIN, const size_t VALUES_MAX,
             const size_t BLOCK_SIZE, const size_t NUM_THREADS) {

    Matrix* C = NULL;
    double* C_blas = NULL;

    // Perform warm-up runs
    for (size_t i = 0; i < WARM_UP_COUNT; i++) {

        // Generate Matrix dimensions
        const size_t n = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t m = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t p = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);

        // Generate matrices to multiply
        Matrix* A = generate_matrix(VALUES_MIN, VALUES_MAX, n, m);
        Matrix* B = generate_matrix(VALUES_MIN, VALUES_MAX, m, p);
        if (!A || !B) {
            perror("Error generate_matrix() failed in warm_up\n");
            matrix_free(A);
            matrix_free(B);
            return -1;
        }

        // Create result matrices (one for this and BLAS implementation)
        if (algo != BLAS) {
            C_blas = NULL;
            C = matrix_create_with(pattern_zero, NULL, n, p);
            if (!C) {
                perror("Error: matrix_create_with() failed for warm-up\n");
                matrix_free(A);
                matrix_free(B);
                return -1;
            }
        } else {
            C = NULL;
            C_blas = (double*)malloc(sizeof(double) * n * p);
            if (!C_blas) {
                perror("Error: Allocation of C_blas failed for warm-up\n");
                matrix_free(A);
                matrix_free(B);
                return -1;
            }
        }

        // Run Matrix multiplication with the desired algorithm
        run_algorithm(algo, A, B, C, C_blas, BLOCK_SIZE, NUM_THREADS, n, m, p);

        // Free the allocated data corresponding the run
        matrix_free(A);
        matrix_free(B);
        if (algo != BLAS) {
            matrix_free(C);
        } else {
            free(C_blas);
        }
    }

    return 0;
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
    double elapsed_time = 0;

    // Algorithm names for printing
    const char* ALGORITHM_NAMES[] = {
        "BLAS",
        "NAIVE",
        "SINGLETHREAD",
        "MULTITHREAD",
        "MULTITHREAD_9AVX"
    };

    // Prepare result matrices (one for this and BLAS implementation)
    Matrix* C = NULL;
    double* C_blas = NULL;

    // Perform the benchmark on each algorithm
    for (size_t i = 0; i <= MULTITHREAD_9AVX; i++) {

        // Retrieve algorithm
        Algorithm algo = (Algorithm)i;

        // Perform the warm-up
        int result = warm_up(WARM_UP_COUNT, algo, DIMENSIONS_MIN, DIMENSIONS_MAX, VALUES_MIN, VALUES_MAX, BLOCK_SIZE, NUM_THREADS);
        if (result != 0) {
            perror("Error: Warm-up has failed\n");
            return -1;
        }

        bool mismatch_detected = true;
        double total_time = 0;

        // Run the benchmark
        for (size_t i = 0; i < RUN_COUNT; i++) {

            printf("%s %s. %s %zu\n", "Algorithm", ALGORITHM_NAMES[algo], "Iteration", i);

            // Generate Matrix dimensions
            const size_t n = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
            const size_t m = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
            const size_t p = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);

            // Generate matrices
            Matrix* A = generate_matrix(VALUES_MIN, VALUES_MAX, n, m);
            Matrix* B = generate_matrix(VALUES_MIN, VALUES_MAX, m, p);
            if (!A || !B) {
                perror("Error generate_matrix() failed in benchmark\n");
                matrix_free(A);
                matrix_free(B);
                return -1;
            }

            // Create result matrices (one for this and BLAS implementation)
            if (algo != BLAS) {
                C_blas = NULL;
                C = matrix_create_with(pattern_zero, NULL, n, p);
                if (!C) {
                    perror("Error: matrix_create_with() failed for benchmark\n");
                    matrix_free(A);
                    matrix_free(B);
                    return -1;
                }
            } else {
                C = NULL;
                C_blas = (double*)malloc(sizeof(double) * n * p);
                if (!C_blas) {
                    perror("Error: Allocation of C_blas failed for benchmark\n");
                    matrix_free(A);
                    matrix_free(B);
                    return -1;
                }
            }

            // Start timer
            clock_gettime(CLOCK_MONOTONIC, &start);

            // Perform the Matrix multiplication
            run_algorithm(algo, A, B, C, C_blas, BLOCK_SIZE, NUM_THREADS, n, m, p);

            // End timer
            clock_gettime(CLOCK_MONOTONIC, &end);

            // Calculate elapsed time in seconds and accumulate
            elapsed_time = (end.tv_sec - start.tv_sec) +
                (end.tv_nsec - start.tv_nsec) / 1e9;
            total_time += elapsed_time;

            // Free the generated matrices
            matrix_free(A);
            matrix_free(B);
            if (algo != BLAS) {
                matrix_free(C);
            } else {
                free(C_blas);
            }
        } // End of single benchmark

        // Calculate average time
        double average_time = total_time / RUN_COUNT;

        // TODO write time to file
    }

    printf("%s\n", "--------FINISHED matrix_mult_benchmark.c--------");

    return 0;
}

