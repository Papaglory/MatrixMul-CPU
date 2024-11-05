#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include "matrix.h"

/**
 * @brief Retrieve a random integer between min and max.
 *
 * @param min The smallest integer value (inclusive).
 * @param max The largest integer value (inclusive).
 * @return An integer.
 */
long random_between(long min, long max);

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
 * @return A pointer to the generated Matrix, NULL if an error occured.
 */
Matrix* generate_matrix(long VALUES_MIN, long VALUES_MAX,
                        size_t NUM_ROWS, size_t NUM_COLS);

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
 * @return Returns 0 for success and -1 if an error occurred.
 */
int matrix_mult_openblas(double *A, double *B, double *C, size_t n, size_t m, size_t p);

/**
 * @brief Determines the smallest of the two input integer values.
 * @param a The first integer.
 * @param b The second integer.
 *
 * @return The integer that is the smallest.
*/
long min(long a, long b);

#endif // MATRIX_UTILS_H
