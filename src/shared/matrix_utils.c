#include "matrix_utils.h"
#include <errno.h>
#include <stdlib.h>
#include <cblas.h>

long random_between(long min, long max) {

     return rand() % (max + 1 - min) + min;
}

Matrix* generate_matrix(long VALUES_MIN, long VALUES_MAX,
                        size_t NUM_ROWS, size_t NUM_COLS) {

    /*
    * Generate Matrix parameters
    */
    int* min_max = (int*)malloc(sizeof(int) * 2);
    int min = random_between(VALUES_MIN, VALUES_MAX);
    int max = random_between(min, VALUES_MAX);
    min_max[0] = min;
    min_max[1] = max;

    // Generate and return Matrix B
    return matrix_create_with(pattern_random_between, min_max, NUM_ROWS, NUM_COLS);
}

void matrix_mult_openblas(double *A, double *B, double *C, size_t n, size_t m, size_t p) {

    if (!A || !B || !C || n <= 0 || m <= 0 || p <= 0) {
        errno = EINVAL;
        perror("Error: Argument for matrix_mult_openblas() are invalid");
        return;
    }

    // cblas_dgemm uses the formula: C = alpha * A x B + beta * C
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                n, p, m,
                1.0,    // Alpha scalar of A
                A, m,   // Leading dimension of A
                B, p,   // Leading dimension of B
                0.0,    // Beta scalar of C
                C, p);  // Leading dimension of C
}

long min(long a, long b) {
    return (a < b) ? a : b;
}
