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

Matrix* matrix_mult_naive(Matrix* A, Matrix* B);

#endif // MATRIX_MULT_NAIVE_H
