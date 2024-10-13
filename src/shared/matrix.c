#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include "matrix.h"

Matrix* matrix_create_from_array(
    size_t num_rows,
    size_t num_cols,
    int arr[num_rows][num_cols]
);

Matrix* matrix_create_zero(size_t num_rows, size_t num_cols) {

    // Both dimensions has to be at greater than 0
    if (num_rows == 0 || num_cols == 0) { errno = EINVAL; return NULL; }

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

        // Initialize row values to 0
        for (size_t j = 0; j < num_cols; j++) {
            rows[i][j] = 0;
        }
    }

    // Check if the loop completed successfully
    if (i != num_rows) {
        // Free everything that was allocated
        for (size_t j = 0; j < i; j++) {
            free(rows[j]);
        }
        free(rows);
        return NULL;
    }

    // Create the matrix
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    if (!m) {
        // free allocated rows before returning
        for (size_t i = 0; i < num_rows; i++) {
            free(rows[i]);
        }
        free(rows);

        errno = ENOMEM;
        return NULL;
    }

    // Set Matrix member variables
    m->rows = rows;
    m->num_rows = num_rows;
    m->num_cols = num_cols;

    return m;
}
