#include <stdio.h>
#include <cblas.h>
#include <stdlib.h>
#include <time.h>
#include "../src/shared/matrix.h"
#include "../src/cpu/matrix_singlethread.h"

/**
 * @brief Retrieve a random integer between min and max.
 *
 * @param min The smallest integer value (inclusive).
 * @param max The largest integer value (inclusive).
 * @return An integer.
 */
int random_between(int min, int max) {

     return rand() % (max + 1 - min) + min;
}

Matrix* generate_matrix(int VALUES_MIN, int VALUES_MAX,
                        size_t num_rows, size_t num_cols) {

    /*
    * Generate Matrix parameters
    */
    int* min_max = (int*)malloc(sizeof(int) * 2);
    int min = random_between(VALUES_MIN, VALUES_MAX);
    int max = random_between(min, VALUES_MAX);
    min_max[0] = min;
    min_max[1] = max;

    // Generate and return Matrix B
    return matrix_create_with(pattern_random_between, min_max, num_rows, num_cols);
}


void verification_test(void *(mult(Matrix* A, Matrix* B))) {

}

void benchmark_test() {

    // Benchmark parameters
    const int RUN_COUNT = 1;
    const int WARMUP_COUNT = 1;
    const int AVERAGE_COUNT = 1;

    // Matrix generation parameters
    const int VALUES_MIN = -1000;
    const int VALUES_MAX = 1000;
    const size_t DIMENSIONS_MIN = 1;
    const size_t DIMENSIONS_MAX = 10;

    struct timespec start, end;
    double elapsed_time;

    double total_time = 0;
    for (size_t i = 0; i < RUN_COUNT; i++) {

        // Generate Matrix dimensions
        const size_t n = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t m = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t p = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);

        // Generate matrices
        Matrix* A = generate_matrix(VALUES_MIN, VALUES_MAX, n, m);
        Matrix* B = generate_matrix(VALUES_MIN, VALUES_MAX, m, p);


        // Perform warmup
        for (size_t j = 0; j < WARMUP_COUNT; j++) {

            //mult(A, B);
        }

        double time = 0;
        for (size_t j = 0; j < AVERAGE_COUNT; j++) {

            clock_gettime(CLOCK_MONOTONIC, &start);
            //mult(A, B);
            clock_gettime(CLOCK_MONOTONIC, &end);

            // Calculate elapsed time in seconds
            elapsed_time = (end.tv_sec - start.tv_sec) +
                           (end.tv_nsec - start.tv_nsec) / 1e9;
            time += elapsed_time;
        }

        total_time += time / AVERAGE_COUNT;
    }
}

double* matrix_to_openblas_matrix(Matrix* m) {

    if (!m) {
        perror("Error: There is no Matrix to convert to openblas");
        return NULL;
    }

    double* openblas_values = (double*)malloc(sizeof(double) * m->num_rows * m->num_cols);
    if (!openblas_values) {
        perror("Error: Allocation failed");
        return NULL;
    }

    // Convert Matrix elements into double
    for (size_t i = 0; i < m->num_rows * m->num_cols; i++) {

        openblas_values[i] = (double)m->values[i];
    }

    return openblas_values;
}

/**
 * @brief Wrapper function of the OpenBLAS matrix multiplication library.
 *
 * @param A 1D array containing the elements of matrix A.
 * @param B 1D array containing the elements of matrix B.
 * @param C 1D array containing the elements of matrix C.
 * @param n The row dimension of A and C.
 * @param m The column dimension of A and row dimension of B.
 * @param p The column dimension of B and C.
 *
 */
void matrix_mult_openblas(double *A, double *B, double *C, int n, int m, int p) {

    // cblas_dgemm uses the formula: C = alpha * A x B + beta * C
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                n, p, m,
                1.0,    // Alpha scalar of A
                A, m,   // Leading dimension of A
                B, p,   // Leading dimension of B
                0.0,    // Beta scalar of C
                C, p);  // Leading dimension of C
}

int main() {

    printf("%s\n", "--------STARTING matrix_mult_verification.c--------");

    // Benchmark parameters
    const int RUN_COUNT = 1;
    const int BLOCK_SIZE = 1; // Does not matter since we only care about result

    // Matrix generation parameters
    const int VALUES_MIN = -1000;
    const int VALUES_MAX = 1000;
    const size_t DIMENSIONS_MIN = 1;
    const size_t DIMENSIONS_MAX = 10;

    struct timespec start, end;
    double elapsed_time;

    double total_time = 0;
    for (size_t i = 0; i < RUN_COUNT; i++) {

        // Generate Matrix dimensions
        const size_t n = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t m = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t p = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);

        // Generate matrices
        Matrix* A = generate_matrix(VALUES_MIN, VALUES_MAX, n, m);
        Matrix* B = generate_matrix(VALUES_MIN, VALUES_MAX, m, p);

        Matrix* C = matrix_singlethread_mult(A, B, 1);

    }

    return 0;
}
