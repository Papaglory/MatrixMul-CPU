/**matrix.h
 *
 * TODO FIX THIS:
 *
 * We use double* instead of double**, that is, internally for the Matrix
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
     * A pointer pointing to the double values on the heap.
     * TODO consider making it immutable.
    */
    double* values;

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
    const double values[num_rows * num_cols]
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
    const double values[num_rows][num_cols]
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
    double* values
);

/**
 * @brief A pattern used by matrix_create_with() to
 * fill the internal Matrix array with zeros.
 *
 * @note args is not used in this pattern.
 *
 * @param values The internal array of the Matrix to fill with zeros.
 * @param args Pointer containing additional arguments. Not used
 * in this function.
 * @param num The number of zeros to insert.
 * @return Return the pointer to values.
 */
double* pattern_zero(double* values, void* args, size_t num);

/**
 * @brief A pattern used by matrix_create_with() to
 * fill the internal Matrix array with random integers
 * between min and max (inclusive) which are variables found
 * in args.
 *
 * @param values The internal array of the Matrix to fill with random doubles.
 * @param args Pointer containing the additional arguments min
 * and max. Variable min and max are the lower and upper bound (inclusive).
 *
 * @return Return the pointer to values.
 */
double* pattern_random_between(double* values, void* args, size_t num);

/**
 * @brief Create a Matrix given the pattern in the function argument.
 * Available patterns: pattern_zero, pattern_random_between.
 *
 * @param pattern A function pointer to the pattern to be used during
 * Matrix generation.
 * @param args Arguments for the pattern function.
 * @param num_rows The number of rows in the Matrix.
 * @param num_cols The number of columns in the Matrix.
 * @return A pointer to the created Matrix object, or NULL if an error occured.
*/
Matrix* matrix_create_with(
    double* (*pattern)(double* values, void* args, size_t num),
    void* args, size_t num_rows, size_t num_cols);

/**
 * @brief Print the given Matrix for visualization or debugging.
 *
 * @details
 * To increase performance, we will not use printf() as this will
 * potentially increase the number of syscalls and overhead.
 * A buffer holds all the characters used to represent the Matrix.
 * An element is represented using either scientific or "normal"
 * notation.
 * This buffer will be flushed once using a read() syscall through
 * invoking sprintf().
 *
 * Worst-case is when every matrix element uses scientific notation with
 * an exponent taking up 4 spaces (3 for the value and 1 for the sign).
 * Required spaces:
 * 1 for sign
 * 1 for e
 * 1 for dot
 * FLOAT_PRECISION for digits after the dot
 * 4 for the exponent value
 * 1 for mantissa before dot
 * 1 for space after the scientific notation (for separating elements)
 * TOTAL: 11 spaces (with FLOAT_PRECISION = 2).
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
