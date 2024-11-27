#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "matrix_singlethread.h"
#include "../shared/matrix.h"
#include "../shared/matrix_utils.h"

void matrix_singlethread_mult(Matrix* A, Matrix* B, Matrix* C, size_t block_size) {

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

    size_t min_nm = min(n, m);
    if (block_size == 0) {
        errno = EINVAL;
        perror("Error: Block size cannot be of value 0");
        return;
    }

    // Check if the block size needs to be adjusted for smaller matrices
    size_t smallest_dimension = min(min_nm, p);
    block_size = (block_size > smallest_dimension) ? smallest_dimension : block_size;

    // Create a new Matrix that is the transpose of Matrix B
    double* B_trans_arr = NULL;
    int result = posix_memalign((void**)&B_trans_arr, 64, sizeof(double) * B->num_rows * B->num_cols);
    if (result != 0) {
        perror("Error: Allocation of B transposed array failed");
        return;
    }

    // Insert transposed values and Allocate Matrix
    for (size_t i = 0; i < B->num_rows; i++) {
        for (size_t j = 0; j < B->num_cols; j++) {
            B_trans_arr[j * B->num_rows + i] = B->values[i * B->num_cols + j];
        }
    }

    // retrieve internal Matrix arrays
    double* A_arr = A->values;
    double* C_arr = C->values;

    // Iterate over blocks of Matrix C
    for (size_t i = 0; i < n; i += block_size) {
        /* Precompute upper bound to make sure we are
         * either inside the block if it is full or inside
         * the matrix if block is partial.
        */
        size_t i_max = min(i + block_size, n);

        for (size_t j = 0; j < p; j += block_size) {
            // Precompute upper bound
            size_t j_max = min(j + block_size, p);

            /*
            * Iterate over blocks in the shared dimension
            * (rows of A and columns of B).
            */
            for (size_t k = 0; k < m; k += block_size) {
                // Precompute upper bound
                size_t k_max = min(k + block_size, m);

                // Iterate over elements within the current block of C
                for (size_t ii = i; ii < i_max; ii++) {
                    for (size_t jj = j; jj < j_max; jj++) {

                        /*
                        * Calculate the dot product for a single element
                        * in the C block. Indices have been precomputed
                        * to avoid redundant calculations in the inner loop.
                        */
                        size_t kk = 0;
                        size_t c_index = ii * p + jj;
                        size_t a_row_offset = ii * m;
                        size_t b_row_offset = jj * m;
                        for (kk = k; kk + 3 < k_max; kk += 4) {
                            C_arr[c_index] += A_arr[a_row_offset + kk] * B_trans_arr[b_row_offset + kk];
                            C_arr[c_index] += A_arr[a_row_offset + (kk + 1)] * B_trans_arr[b_row_offset + kk + 1];
                            C_arr[c_index] += A_arr[a_row_offset + (kk + 2)] * B_trans_arr[b_row_offset + kk + 2];
                            C_arr[c_index] += A_arr[a_row_offset + (kk + 3)] * B_trans_arr[b_row_offset + kk + 3];
                        }
                        for (; kk < k_max; kk++) {
                            C_arr[c_index] += A_arr[a_row_offset + kk] * B_trans_arr[b_row_offset + kk];
                        }
                    }
                }
            }
        }
    }
    // Free the helper B transpose Matrix
    free(B_trans_arr);
}

