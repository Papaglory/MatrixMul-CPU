#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "matrix_singlethread.h"
#include "../shared/matrix.c"

/**
 * @brief Determines the smallest of the two input integer values.
 * @param a The first integer.
 * @param b The first integer.
 *
 * @return The integer that is the smallest.
*/
int min(int a, int b) {
    return (a < b) ? a : b;
}

Matrix* mult(Matrix* A, Matrix* B, size_t block_size) {

    // Extract Matrix dimensions
    size_t n = A->num_rows;
    size_t m = A->num_cols;
    size_t p = B->num_cols;

    if (!A || !B) {
        // Parameters missing
        errno = EINVAL;
        perror("Error: Missing either Matrix A or Matrix B");
        return NULL;
    }

    if (n == 0 || m == 0 || p == 0) {
        // Invalid Matrix dimensions
        errno = EINVAL;
        perror("Error: At least one of the dimensions (n, m or p) are 0");
        return NULL;
    }

    if (block_size == 0 || block_size > n || block_size > m || block_size > p) {
        // Invalid block size
        errno = EINVAL;
        perror("Error: An invalid block size has been chosen");
        return NULL;
    }

    Matrix* C = matrix_create_with(pattern_zero, NULL, n, p);
    if (!C) { return NULL; }

    int* A_arr = A->values;
    int* B_arr = B->values;
    int* C_arr = C->values;

    // Iterate over blocks of Matrix C
    for (size_t i = 0; i < n; i += block_size) {
        // Precompute upper bound to make sure we are
        // either inside the block if it is full or inside the matrix if block is partial
        size_t i_max = min(i + block_size, n);

        for (size_t j = 0; j < p; j += block_size) {
            // Precompute upper bound
            size_t j_max = min(j + block_size, p);

            // Iterate over blocks in the shared dimension (rows of A and columns of B)
            for (size_t k = 0; k < m; k += block_size) {
                // Precompute upper bound
                size_t k_max = min(k + block_size, m);

                // Iterate over elements within the current block of C
                for (size_t ii = i; ii < i_max; ii++) {
                    for (int jj = j; jj < j_max; jj++) {

                        // Calculate the dot product for a single element in the C block
                        // Precompute the index for the current element in matrix C to avoid redundant calculations in the innermost loop
                        size_t kk = 0;
                        size_t c_index = ii * p + jj;
                        size_t a_row_offset = ii * m;
                        for (kk = k; kk < k_max - 3; kk += 4) {
                            C_arr[c_index] += A_arr[a_row_offset + kk] * B_arr[kk * p + jj];
                            C_arr[c_index] += A_arr[a_row_offset + (kk + 1)] * B_arr[(kk + 1) * p + jj];
                            C_arr[c_index] += A_arr[a_row_offset + (kk + 2)] * B_arr[(kk + 2) * p + jj];
                            C_arr[c_index] += A_arr[a_row_offset + (kk + 3)] * B_arr[(kk + 3) * p + jj];
                        }
                        for (; kk < k_max; kk++) {
                            C_arr[c_index] += A_arr[a_row_offset + kk] * B_arr[kk * p + jj];
                        }
                    }
                }
            }
        }
    }

  // Return the result
  return C;
}
