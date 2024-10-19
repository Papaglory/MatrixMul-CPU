

/**
 *
 * TODO FIX THIS:
 *
 * We use int* instead of int**, that is, internally for the Matrix
 * we have a 1D array and not a 2D array due to optimization.
 * - Contiguous memory instead of fragmented.
 *   This boosts spatial locality and cache optimization.
 *   Also reduces overhead of having a 2D array using pointers.
 *
 */


#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    /**
     * A pointer pointing to the integer values on the heap.
    */
    int* values;

    // The number of rows in the Matrix
    size_t num_rows;
    // The number of columns in the Matrix
    size_t num_cols;
    // true if the Matrix owns the rows and should free them
    bool owns_rows;

} Matrix;

/**
 * @brief Create a Matrix from a static 1D array.
 *
 * @note The implementation is identical to the function
 * matrix_create_from_2D_array(), except for the array insertion logic.
 * These two functions are purposefully kept in separate functions for
 * API clearity and slightly optimized code at the cost of slightly
 * higher binary file size.
 *
 * @param num_rows The number of rows in the Matrix.
 * @param num_cols The number of columns in the Matrix.
 * @param values The array containg the values to insert into the Matrix.
 * @return A pointer to the created Matrix object, or NULL if an error occured.
*/
Matrix* matrix_create_from_1D_array(
    size_t num_rows,
    size_t num_cols,
    int values[num_rows * num_cols]
);

/**
 * @brief Create a Matrix from a static 2D array.
 *
 * @note The implementation is identical to the function
 * matrix_create_from_1D_array(), except for the array insertion logic.
 * These two functions are purposefully kept in separate functions for
 * API clearity and slightly optimized code at the cost of slightly
 * higher binary file size.
 *
 * @param num_rows The number of rows in the Matrix.
 * @param num_cols The number of columns in the Matrix.
 * @param values The array containg the values to insert into the Matrix.
 * @return A pointer to the created Matrix object, or NULL if an error occured.
*/
Matrix* matrix_create_from_2D_array(
    size_t num_rows,
    size_t num_cols,
    int values[num_rows][num_cols]
);

/**
 * @brief Create a Matrix from a pointer pointing to an array on the heap.
 *
 * @note This function trusts that the caller gives a values pointer
 * that points to a valid 1D array with the specified dimensions
 * to prevent deep validation and unnecessary overhead.
 *
 * Furthermore, this function does not take ownership of values. The caller
 * is responsible for freeing the memory allocated for values.
 *
 * @param num_rows The number of rows in the Matrix.
 * @param num_cols The number of columns in the Matrix.
 * @param values A pointer to the allocated array containing the
 * elements of the Matrix.
 * @return A pointer to the created Matrix object, or NULL if an error occured.
*/
Matrix* matrix_create_from_pointers(
    size_t num_rows,
    size_t num_cols,
    int* values
);

/**
 * @brief Create a Matrix given the pattern in the function argument.
 * Available patterns: pattern_zero, pattern_random_between.
 *
 * @param pattern A function pointer to the pattern to be used during
 * Matrix generation.
 * @param num_rows The number of rows in the Matrix.
 * @param num_cols The number of columns in the Matrix.
 * @return A pointer to the created Matrix object, or NULL if an error occured.
*/
Matrix* matrix_create_with(int (*pattern)(), size_t num_rows, size_t num_cols);

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
 * @param m The Matrix to be printed.
 * @return A value of zero for success, a value of -1 if an error occured.
*/
int matrix_print(Matrix* m);

/**
 * @brief Free the allocated Matrix from the heap.
 *
 * @param m1 Pointer to the Matrix.
 * @return A value of zero for success and -1 if an error occured.
*/
int matrix_free(Matrix* m);

#endif // MATRIX_H
