/**
 * @file matrix_multithread.h
 *
 * @brief Contains function prototypes for Matrix multiplication
 * utilizing the following for improved performance:
 * - Multithreading
 * - Blocking / tiling method
 *
 * @Note This implementation does not use SIMD registers. It is
 * default multithread implementation using the blocking / tiling method.
 *
 * @details
 * For the multithreading, a single Queue is created that hands out
 * tasks corresponding to blocks in the C Matrix that needs to be
 * calculated. Each task is wrapped using the Task struct containing
 * the relevant information. To access the Queue, the threads share
 * a single mutex.
 *
 * For documentation on the blocking / tiling method,
 * see matrix_singlethread.h.
 */

#ifndef MATRIX_MULTITHREAD_H
#define MATRIX_MULTITHREAD_H

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
 * @param NUM_THREADS The number of threads to utilize.
*/
void matrix_multithread_mult(Matrix* A, Matrix* B, Matrix* C, size_t block_size, size_t NUM_THREADS);

#endif // MATRIX_MULTITHREAD_H
