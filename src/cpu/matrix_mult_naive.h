/**
 * @file matrix_mult_naive.h
 *
 * @brief An implementation of the naive Matrix multiplication algorithm
 * using three for loops. Naive in the sense that given A x B = C, to
 * calculate c_{ij} we sequentially calculate the dot product sum
 * by going through the entire i-th row in A and j-th column in B.
*/

#ifndef MATRIX_MULT_NAIVE_H
#define MATRIX_MULT_NAIVE_H

#include "../shared/matrix.h"

/**
 * @brief Performs naive matrix multiplication on matrices A and B,
 * storing the result in matrix C.
 *
 * @note Matrix C must be pre-allocated by the caller.
 *
 * @param A Pointer to the first input matrix (dimensions n x m).
 * @param B Pointer to the second input matrix (dimensions m x p).
 * @param C Pointer to the output matrix (dimensions n x p) where
 * the result will be stored.
 */
void matrix_mult_naive(Matrix* A, Matrix* B, Matrix* C);

#endif // MATRIX_MULT_NAIVE_H
