#ifndef MATRIX_MULTITHREAD_H
#define MATRIX_MULTITHREAD_H

#include "../shared/matrix.h"

/**
 * TODO update docs
 * @brief Matrix multiply the two matrices A and B. Matrix A is the
 * left-Matrix and Matrix B is the right-Matrix.
 * Assumes the number of columns in the first Matrix is equal
 * to the number of rows in the second Matrix.
 * The result is placed in a newly allocated Matrix.
 *
 * @param A The first Matrix.
 * @param B The second Matrix.
 * @param block_size The block size used in the blocking / tiling method.
 * @return A pointer to a newly alloced Matrix which contains the result
 * of the multiplication. Returns NULL if an error occured.
*/
Matrix* matrix_multithread_mult(Matrix* A, Matrix* B, size_t block_size);

#endif // MATRIX_MULTITHREAD_H
