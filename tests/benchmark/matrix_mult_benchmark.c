#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../../src/shared/matrix.h"
#include "../../src/cpu/matrix_mult_naive.h"
#include "../../src/cpu/matrix_multithread.h"
#include "../../src/cpu/matrix_multithread_9avx.h"
#include "../../src/cpu/matrix_singlethread.h"
#include "../../src/shared/matrix_utils.h"

// Algorithms to be tested
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
            matrix_multithread_mult_9avx(A, B, C, BLOCK_SIZE, NUM_THREADS);
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
             const double VALUES_MIN, const double VALUES_MAX,
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
            fprintf(stderr, "Error generate_matrix() failed in warm_up\n");
            matrix_free(A);
            matrix_free(B);
            return 1;
        }

        // Create result matrices (one for this and BLAS implementation)
        if (algo != BLAS) {
            C_blas = NULL;
            C = matrix_create_with(pattern_zero, NULL, n, p);
            if (!C) {
                fprintf(stderr, "Error: matrix_create_with() failed for warm-up\n");
                matrix_free(A);
                matrix_free(B);
                return 1;
            }
        } else {
            C = NULL;
            C_blas = (double*)malloc(sizeof(double) * n * p);
            if (!C_blas) {
                fprintf(stderr, "Error: Allocation of C_blas failed for warm-up\n");
                matrix_free(A);
                matrix_free(B);
                return 1;
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

/**
 * @brief Determine if the input string str is a digit.
 *
 * @param str The string under consideration.
 * @return 0 for containing a digit, else 1 for a non-digit string.
 */
int is_integer(const char str[]) {

    // Check that it is not the empty string
    if (*str == '\0') { return 1; }

    // Increment if the first character is a sign
    if (*str == '+' || *str == '-') { str++; }

    // Check that there is at least one character after the sign
    if (*str == '\0') { return 1; }

    // Make sure every character is a digit
    while (*str != '\0') {
        if (!isdigit(*str)) { return 1; }
        str++;
    }

    return 0;
}

int main(int argc, char* argv[]) {

    // Check for input algorithm existence
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <Algorithm> <Dimension_Size> <Seed>\n%s\n", argv[0], "Algorithm Options:\nBLAS\nNAIVE\nSINGLETHREAD\nMULTITHREAD\nMULTITHREAD_9AVX\nContent is stored in benchmark_time.txt");
        return 1;
    }

    // Retrieve input algorithm
    Algorithm algo;
    if (strcmp(argv[1], "BLAS") == 0) {
        algo = BLAS;
    } else if (strcmp(argv[1], "NAIVE") == 0) {
        algo = NAIVE;
    } else if (strcmp(argv[1], "SINGLETHREAD") == 0) {
        algo = SINGLETHREAD;
    } else if (strcmp(argv[1], "MULTITHREAD") == 0) {
        algo = MULTITHREAD;
    } else if (strcmp(argv[1], "MULTITHREAD_9AVX") == 0) {
        algo = MULTITHREAD_9AVX;
    } else {
        // No valid algorithm was given as input
        fprintf(stderr, "Invalid algorithm inputted\n");
        return 1;
    }

    // Retrieve input dimension and seed
    if (is_integer(argv[2]) != 0 || is_integer(argv[3]) != 0) {
        fprintf(stderr, "%s\n", "Error: Input dimension or seed is not a valid integer string");
        return 1;
    }
    const size_t DIMENSION_SIZE = atoi(argv[2]);
    if (DIMENSION_SIZE == 0) {
        fprintf(stderr, "%s\n", "Error: The dimension size has to be non-zero");
        return 1;
    }
    const size_t SEED = atoi(argv[3]);


    printf("%s\n", "--------STARTING matrix_mult_benchmark.c--------");

    // Benchmark parameters
    const size_t RUN_COUNT = 10;
    const size_t WARM_UP_COUNT = 10;
    const size_t BLOCK_SIZE = 128;
    const size_t NUM_THREADS = 16;
    const char filename[] = "benchmark_time.txt";

    // Matrix generation parameters
    const double VALUES_MIN = -1e+6;
    const double VALUES_MAX = 1e+6;
    const size_t DIMENSIONS_MIN = DIMENSION_SIZE;
    const size_t DIMENSIONS_MAX = DIMENSION_SIZE;
    const int seed = SEED;

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

    // Perform the warm-up
    printf("%s\n", "Performing warm-up...");
    int result = warm_up(WARM_UP_COUNT, algo, DIMENSIONS_MIN, DIMENSIONS_MAX, VALUES_MIN, VALUES_MAX, BLOCK_SIZE, NUM_THREADS);
    if (result != 0) {
        fprintf(stderr, "Warm-up has failed\n");
        return 1;
    }

    double total_time = 0;

    // Run the benchmark
    printf("%s\n", "Performing benchmark...");
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
            fprintf(stderr, "Error generate_matrix() failed in benchmark\n");
            matrix_free(A);
            matrix_free(B);
            return 1;
        }

        // Create result matrices (one for this and BLAS implementation)
        if (algo != BLAS) {
            C_blas = NULL;
            C = matrix_create_with(pattern_zero, NULL, n, p);
            if (!C) {
                fprintf(stderr, "Error: matrix_create_with() failed for benchmark\n");
                matrix_free(A);
                matrix_free(B);
                return 1;
            }
        } else {
            C = NULL;
            C_blas = (double*)malloc(sizeof(double) * n * p);
            if (!C_blas) {
                fprintf(stderr, "Error: Allocation of C_blas failed for benchmark\n");
                matrix_free(A);
                matrix_free(B);
                return 1;
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
    }

   // Calculate average time
    double average_time = total_time / RUN_COUNT;
    printf("%s %f\n", "average time", average_time);

    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Opening file for storing time failed\n");
        return 1;
    }
    fprintf(file, "%f\n", average_time);
    fclose(file);

    printf("%s\n", "--------FINISHED matrix_mult_benchmark.c--------");

    return 0;
}

