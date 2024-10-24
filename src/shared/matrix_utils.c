#include "matrix_utils.h"
#include <stdlib.h>
#include <cblas.h>

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

/**
 * @brief Generate a Matrix with random elements given the parameters
 * found in the function signature.
 *
 * @param VALUES_MIN The minimum possible value an element can be.
 * @param VALUES_MAX The maximum possible value an element can be.
 * @param NUM_ROWS The maximum possible number of rows the generated
 * Matrix can have.
 * @param NUM_COLS The maximum possible number of columns the generated
 * Matrix can have.
 *
 * @return A pointer to the generated Matrix.
 */
Matrix* generate_matrix(int VALUES_MIN, int VALUES_MAX,
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

/**
 * @brief Wrapper function of the OpenBLAS matrix multiplication library.
 *
 * @param A 1D array containing the elements of matrix A.
 * @param B 1D array containing the elements of matrix B.
 * @param C 1D array containing the elements of matrix C.
 * @param n The row dimension of A and C.
 * @param m The column dimension of A and row dimension of B.
 * @param p The column dimension of B and C.
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

int min(int a, int b) {
    return (a < b) ? a : b;
}
