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

/**
 * @brief Create a Matrix from a static array given by the user.
 *
 * @param num_rows The number of rows in the Matrix.
 * @param num_cols The number of columns in the Matrix.
 * @param values The array containg the values of the Matrix.
 * @return A pointer to the created Matrix object, or NULL if an error occured.
*/
Matrix* matrix_create_from_array(
    size_t num_rows,
    size_t num_cols,
    int values[num_rows][num_cols]
);

/*
 * @brief Create a Matrix from a pointer of pointers (int**).
 *
 * @note The function trusts that the caller gives a 'rows' pointer
 * that points to a valid 2D array with the specified dimensions,
 * to prevent deep validation and unnecessary overhead.
 *
 * Furthermore, this function does not take ownership of 'rows'. The caller
 * is responsible for freeing the memory allocated for rows.
 *
 * @param num_rows The number of rows in the Matrix.
 * @param num_cols The number of columns in the Matrix.
 * @param rows A pointer to the allocated array containing the
 * values of the Matrix.
 * @return A pointer to the created Matrix object, or NULL if an error occured.
*/
Matrix* matrix_create_from_pointers(
    size_t num_rows,
    size_t num_cols,
    int** rows
);

/*
 * @brief Create a Matrix containing all zeros.
 *
 * @param num_rows The number of rows in the Matrix.
 * @param num_cols The number of columns in the Matrix.
 * @return A pointer to the created Matrix object, or NULL if an error occured.
*/
Matrix* matrix_create_zero(size_t num_rows, size_t num_cols);

Matrix* matrix_add(void* matrix_1, void* matrix_2);

Matrix* matrix_mult(void* matrix_1, void* matrix_2);

#endif // MATRIX_H
