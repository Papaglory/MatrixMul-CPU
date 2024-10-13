#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include "matrix.h"

/**
 * @brief Given a pointer that represents an array of arrays / 2D array
 * of type (int**), free each row / array and finally free the 'rows' pointer.
 *
 * @param num_rows The number of rows in the 2D array.
 * @param rows The pointer pointing to the array of arrays / 2D array.
 * @return A zero to indicate success, and -1 if an error occured.
*/
int free_rows(size_t num_rows, int** rows) {

    // Check for valid arguments
    if (num_rows == 0 || !rows) { errno = EINVAL; return -1; }

    for (size_t i = 0; i < num_rows; i++) {
        free(rows[i]);
    }
    free(rows);
    return 0;
}

/**
 * @brief Given the input dimensions, allocate an array of arrays / 2D array
 * on the heap.
 *
 * @param num_rows The number of rows in the 2D array.
 * @param num_cols The number of cols in the 2D array.
 * @return A pointer to the allocated 2D array on the heap, and NULL
 * if an error occured.
*/
int** allocate_rows(size_t num_rows, size_t num_cols) {

    // Allocate memory for the row pointers
    int** rows = (int**)malloc(num_rows * sizeof(int*));
    if (!rows) { errno = ENOMEM; return NULL; }

    // Allocate each row in rows
    size_t i = 0;
    for (i = 0; i < num_rows; i++) {
        rows[i] = (int*)malloc(num_cols * sizeof(int));
        if (!rows[i]) {
            // Memory allocation failed
            errno = ENOMEM;
            break;
        }
    }

    // Check if the loop completed successfully
    if (i != num_rows) {
        // Free the rows that was allocated
        free_rows(i, rows);
        return NULL;
    }

    return rows;
}

Matrix* matrix_create_from_array(size_t num_rows, size_t num_cols,
                                int values[num_rows][num_cols]) {

    // Both dimensions has to be at greater than 0
    if (num_rows == 0 || num_cols == 0) { errno = EINVAL; return NULL; }

    // Allocate a 2D array with size determined by 'num_rows', 'num_cols'
    int** rows = allocate_rows(num_rows, num_cols);
    if (!rows) { return NULL; }

    // Initialize row values according to user-array
    for (size_t i = 0; i < num_rows; i++) {
        for (size_t j = 0; j < num_cols; j++) {
            rows[i][j] = values[i][j];
        }
    }

    // Create the Matrix
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    if (!m) {
        // free allocated rows before returning
        free_rows(num_rows, rows);
        return NULL;
    }

    // Set Matrix member variables
    m->rows = rows;
    m->num_rows = num_rows;
    m->num_cols = num_cols;

    return m;
}

Matrix* matrix_create_zero(size_t num_rows, size_t num_cols) {

    // Both dimensions has to be at greater than 0
    if (num_rows == 0 || num_cols == 0) { errno = EINVAL; return NULL; }

    // Allocate a 2D array with size determined by 'num_rows', 'num_cols'
    int** rows = allocate_rows(num_rows, num_cols);
    if (!rows) { return NULL; }

    // Initialize row values to zeros
    for (size_t i = 0; i < num_rows; i++) {
        for (size_t j = 0; j < num_cols; j++) {
            rows[i][j] = 0;
        }
    }

    // Create the Matrix
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    if (!m) {
        // free allocated rows before returning
        free_rows(num_rows, rows);
        return NULL;
    }

    // Set Matrix member variables
    m->rows = rows;
    m->num_rows = num_rows;
    m->num_cols = num_cols;

    return m;
}
