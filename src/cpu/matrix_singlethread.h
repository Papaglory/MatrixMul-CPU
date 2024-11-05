/**
 * @file matrix_singlethread.h
 * @brief Matrix Multiplication for Single Thread
 * This file defines a Matrix multiplication algorithm based
 * on the Matrix struct in matrix.h.
 *
 * The algorithm utilizes the blocking / tiling method to better
 * utilize cache lines loaded into the CPU, thus improving cache
 * hits compared to the simplified matrix multiplication algorithm.
 *
 * @details
 * Suppose we have matrices A, B and C = A x B.
 * A has n rows and m columns and C has p columns.
 *
 * In order to calculate an element c_{ij} in C, we need the entire
 * entire i-th row in A and j-th column in B. In the C language, arrays
 * are stored in a row-major order. This means that the elements of
 * a multi-dimensional array are stored in memory row by row. This
 * means that the i-th row in A will likely have more cache hits
 * compared to the j-th column in B which is not cache friendly
 * as we only use one element per cache line we retrieve from memory.
 * This will most likely reduce performance.
 *
 * By using the blocking method, we utilize more of the elements
 * in the B rows instead of just a single element. Furthermore,
 * since the blocking method considers smaller blocks than the
 * whole Matrix at once, with correct tuning, we might be able
 * to fit the whole block within the CPU's L1, L2 and L3 caches.
 */

#ifndef MATRIX_SINGLETHREAD_H
#define MATRIX_SINGLETHREAD_H

#include "../shared/matrix.h"

/**
 * @brief Matrix multiply the two matrices A and B. Matrix A is the
 * left-Matrix and Matrix B is the right-Matrix.
 *
 * @note Matrix C must be pre-allocated by the caller.
 *
 * @param A Pointer to the first input Matrix (dimensions n x m).
 * @param B Pointer to the second input Matrix (dimensions m x p).
 * @param C Pointer to the output Matrix (dimensions n x p) where
 * the result will be stored.
 * @param block_size The block size used in the blocking / tiling method.
*/
void matrix_singlethread_mult(Matrix* A, Matrix* B, Matrix* C, size_t block_size);

#endif //MATRIX_SINGLETHREAD_H
