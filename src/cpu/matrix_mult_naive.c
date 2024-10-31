#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "matrix_mult_naive.h"

Matrix* matrix_mult_naive(Matrix* A, Matrix* B) {

    if (!A || !B) {
        errno = EINVAL;
        perror("Error: Missing either Matrix A or Matrix B");
        return NULL;
    }

    // Extract Matrix dimensions
    size_t n = A->num_rows;
    size_t m = A->num_cols;
    size_t p = B->num_cols;

    if (n == 0 || m == 0 || p == 0) {
        errno = EINVAL;
        perror("Error: At least one of the dimensions (n, m or p) are 0");
        return NULL;
    }

    // Instantiate Matrix C
    Matrix* C = matrix_create_with(pattern_zero, NULL, n, p);
    if (!C) {
        errno = EINVAL;
        perror("Error: Unable to allocate memory for Matrix C");
        return NULL;
    }

    // retrieve internal Matrix arrays
    double* A_arr = A->values;
    double* B_arr = B->values;
    double* C_arr = C->values;

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < p; j++) {

            // Calculate the dot product for this cell in C
            for (size_t k = 0; k < m; k++) {
                C_arr[i * p + j] += A_arr[i * m + k] * B_arr[k * p + j];
            }
        }
    }

    // Return the result
    return C;
}
