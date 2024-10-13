#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

typedef struct {
    /*
     * A pointer pointing to a pointer that points to an integer.
     * The idea of an array on the heap is handled through the
     * use of pointers.
    */
    int** rows;

    size_t num_rows;
    size_t num_cols;

} Matrix;

/*
 * @brief Create a Matrix from a static array given by the user.
 *
 * @param1 The number of rows in the Matrix.
 * @param2 The number of columns in the Matrix.
 * @param3 The array containg the values of the Matrix.
 * @return A Matrix object.
*/
Matrix* matrix_create_from_array(
    size_t num_rows,
    size_t num_cols,
    int arr[num_rows][num_cols]
);

/*
 * @brief Create a Matrix from a pointer of pointers (int**).
 *
 * @param1 The number of rows in the Matrix.
 * @param2 The number of columns in the Matrix.
 * @param3 A pointer to the allocated array containing the values of the Matrix.
 * @return A Matrix object.
*/
Matrix* matrix_create_from_pointers(
    size_t num_rows,
    size_t num_cols,
    int** rows
);

/*
 * @brief Create a Matrix containing all zeros.
 *
 * @param1 The number of rows in the Matrix.
 * @param2 The number of columns in the Matrix.
 * @return A Matrix object.
*/
Matrix* matrix_create_zero(size_t num_rows, size_t num_cols);

Matrix* matrix_add(void* matrix_1, void* matrix_2);

Matrix* matrix_mult(void* matrix_1, void* matrix_2);

#endif // MATRIX_H
