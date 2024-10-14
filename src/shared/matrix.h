#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

typedef struct {
    /**
     * A pointer pointing to a pointer that points to an integer.
     * The idea of an array on the heap is handled through the
     * use of pointers.
    */
    int** rows;

    // The number of rows in the Matrix
    size_t num_rows;
    // The number of columns in the Matrix
    size_t num_cols;
    // true if the Matrix owns the rows and should free them
    bool owns_rows;

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

/**
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

/**
 * @brief Create a Matrix containing all zeros.
 *
 * @param num_rows The number of rows in the Matrix.
 * @param num_cols The number of columns in the Matrix.
 * @return A pointer to the created Matrix object, or NULL if an error occured.
*/
Matrix* matrix_create_zero(size_t num_rows, size_t num_cols);

/**
 * @brief Add the two matrices by adding the elements together.
 * Assumes the matrices have the same dimensions. The result
 * is placed in a newly allocated Matrix.
 * Vanilla means it is single threaded on the CPU.
 *
 * @param m1 The first Matrix.
 * @param m2 The second Matrix.
 * @return A pointer to a newly alloced Matrix which contains the result
 * of the additon. Returns NULL if an error occured.
*/
Matrix* matrix_vanilla_add(void* m1, void* m2);

/**
 * @brief Matrix multiply the two matrices. Matrix 'm1' is the
 * left-Matrix and Matrix 'm2' is the right-Matrix.
 * Assumes the number of columns in the first Matrix is equal
 * to the number of rows in the second Matrix.
 * The result is placed in a newly allocated Matrix.
 * Vanilla means it is single threaded on the CPU.
 *
 * @param m1 The first Matrix.
 * @param m2 The second Matrix.
 * @return A pointer to a newly alloced Matrix which contains the result
 * of the multiplication. Returns NULL if an error occured.
*/
Matrix* matrix_vanilla_mult(void* m1, void* m2);

/**
 * @brief Print the given Matrix for visualization or debugging.
 *
 * @details
 * To increase performance, we will not use printf() as this will
 * potentially increase the number of syscalls and overhead.
 * A buffer holds all the characters used to represent the Matrix.
 * With each element being an integer, an element can take on a value
 * between 2,147,483,647 and -2,147,483,647. To represent
 * this range in characters we need 11 bytes per element (one for space).
 * We add the number of rows for the new line characters.
 * This buffer will be flushed once using a read() syscall through
 * invoking sprintf().
 *
 * @param m The matrix to be printed.
 * @return A value of zero for success, a value of -1 if an error occured.
*/
int matrix_print(Matrix* m);

/**
 * @brief Free the allocated Matrix from the heap.
 *
 * @param m1 Pointer to the Matrix.
 * @return A value of zero for success and zero if an error occured.
*/
int matrix_free(Matrix* m);

#endif // MATRIX_H
