#ifndef MATRIX_MULTITHREAD_H
#define MATRIX_MULTITHREAD_H

#include "../shared/matrix.h"

/**
 * @brief Add the two matrices by adding the elements together.
 * Assumes the matrices have the same dimensions. The result
 * is placed in a newly allocated Matrix.
 *
 * @param m1 The first Matrix.
 * @param m2 The second Matrix.
 * @return A pointer to a newly alloced Matrix which contains the result
 * of the additon. Returns NULL if an error occured.
*/
Matrix* matrix_multithread_add(void* m1, void* m2);

/**
 * @brief Matrix multiply the two matrices. Matrix 'm1' is the
 * left-Matrix and Matrix 'm2' is the right-Matrix.
 * Assumes the number of columns in the first Matrix is equal
 * to the number of rows in the second Matrix.
 * The result is placed in a newly allocated Matrix.
 *
 * @param m1 The first Matrix.
 * @param m2 The second Matrix.
 * @return A pointer to a newly alloced Matrix which contains the result
 * of the multiplication. Returns NULL if an error occured.
*/
Matrix* matrix_multithread_mult(void* m1, void* m2);

#endif // MATRIX_MULTITHREAD_H
