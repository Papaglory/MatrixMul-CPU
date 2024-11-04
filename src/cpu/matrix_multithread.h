/**
 * @file matrix_multithread.h
 *
 * @brief Contains function prototypes for Matrix multiplication
 * utilizing the following for improved performance:
 * - Multithreading
 * - SIMD registers
 * - Blocking / tiling method
 *
 * @details
 * For the multithreading, a single Queue is created that hands out
 * tasks corresponding to blocks in the C Matrix that needs to be
 * calculated. Each task is wrapped using the Task struct containing
 * the relevant information. To access the Queue, the threads share
 * a single mutex.
 *
 * For documentation on the blocking / tiling method,
 * see matrix_multithread.h.
 */

#ifndef MATRIX_MULTITHREAD_H
#define MATRIX_MULTITHREAD_H

#include "../shared/matrix.h"

/**
 * @brief Matrix multiply the two matrices A and B. Matrix A is the
 * left-Matrix and Matrix B is the right-Matrix.
 *
 * @note The result is placed in a newly allocated Matrix and the
 * caller is responsible to free.
 *
 * @param A The first Matrix.
 * @param B The second Matrix.
 * @param block_size The block size used in the blocking / tiling method.
 * @param NUM_THREADS The number of threads to utilize.
 * @return A pointer to a newly alloced Matrix which contains the result
 * of the multiplication. Returns NULL if an error occured.
*/
Matrix* matrix_multithread_mult(Matrix* A, Matrix* B, size_t block_size, size_t NUM_THREADS);

#endif // MATRIX_MULTITHREAD_H
