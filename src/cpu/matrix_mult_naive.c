#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "matrix_mult_naive.h"

void matrix_mult_naive(Matrix* A, Matrix* B, Matrix* C) {

    if (!A || !B || !C) {
        errno = EINVAL;
        perror("Error: Missing either Matrix A, B or Matrix C");
        return;
    }

    // Extract Matrix dimensions
    size_t n = A->num_rows;
    size_t m = A->num_cols;
    size_t p = B->num_cols;

    if (n == 0 || m == 0 || p == 0) {
        errno = EINVAL;
        perror("Error: At least one of the dimensions (n, m or p) are 0");
        return;
    }

    // Check if Matrix multiplication is valid given matrices
    if (A->num_cols != B->num_rows ||
        C->num_rows != A->num_rows ||
        C->num_cols != B->num_cols) {
        errno = EINVAL;
        perror("Error: Matrix dimensions are not valid for multiplication\n");
        return;
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
}
