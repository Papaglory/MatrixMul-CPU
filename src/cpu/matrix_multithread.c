#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include "matrix_multithread.h"
#include "../shared/matrix.h"
#include "../shared/queue.h"
#include "../shared/matrix_utils.h"

/**
 * @brief Helper function for matrix_multithread_mult(). It allocates a
 * Matrix B transposed for the Matrix calculations. This Matrix is
 * freed at the end of matrix_multithread_mult(). This function also
 * establishes a Queue object and fills it with Task objects that
 * reflect each subjob / block that needs to be calculated in Matrix C.
 *
 * @param A Pointer to Matrix A (A x B = C).
 * @param B Pointer to Matrix B.
 * @param C Pointer to Matrix C.
 * @param block_size The block size used in the blocking / tiling method.
 * @return Pointer to the Queue.
*/
Queue* preprocessing(Matrix* A, Matrix* B, Matrix* C, size_t block_size) {

    // Create a new Matrix that is the transpose of Matrix B
    double* B_trans_arr = (double*)malloc(sizeof(double) * B->num_rows * B->num_cols);
    if (!B_trans_arr) {
        perror("Error: Allocation of B transposed array failed");
        return NULL;
    }

    // Insert transposed values and Allocate Matrix
    for (size_t i = 0; i < B->num_rows; i++) {
        for (size_t j = 0; j < B->num_cols; j++) {
            B_trans_arr[j * B->num_rows + i] = B->values[i * B->num_cols + j];
        }
    }
    Matrix* B_trans = matrix_create_from_pointers(B->num_cols, B->num_rows, B_trans_arr);

    // Extract Matrix dimensions for C
    size_t n = A->num_rows;
    size_t p = B->num_cols;

    // Determine if we have edge cases when blocking / tiling Matrix C
    bool perfect_row = false;
    bool perfect_col = false;
    if (n % block_size == 0) { perfect_row = true; }
    if (p % block_size == 0) { perfect_col = true; }

    /*
     * The number of full blocks along each dimension in C.
     * Division with size_t floors the value to closest whole number.
     */
    size_t full_row_blocks = n / block_size;
    size_t full_col_blocks = p / block_size;
    size_t full_total_blocks = full_row_blocks * full_col_blocks;

    // Holds the number of blocks / tasks in C for Queue creation
    size_t num_tasks = 0;

    // Depending on edge cases, determine the total blocks in Matrix C
    if (perfect_row && perfect_col) {
        num_tasks = full_total_blocks;
    } else if (perfect_row && !perfect_col) {
        num_tasks = full_total_blocks + full_row_blocks;
    } else if (!perfect_row && perfect_col) {
        num_tasks = full_total_blocks + full_col_blocks;
    } else {
        // No perfects
        num_tasks = full_total_blocks + full_row_blocks + full_col_blocks + 1;
    }

    // Set up Queue
    Queue* q = queue_create(num_tasks);

    // Turn each block in C into a Task for the Queue
    for (size_t i = 0; i < n; i += block_size) {
        for (size_t j = 0; j < p; j += block_size) {

            // These make sure we do not leave Matrix C due to edge cases
            size_t i_max = min(i + block_size, n);
            size_t j_max = min(j + block_size, p);

            // Store the block inside a Task and enqueue it
            Task t = task_create(A, B_trans, C, block_size, i, j, i_max, j_max);
            queue_add(q, t);
        }
    }

    return q;
}

/**
 * @brief Helper function to task_worker(). This function encapsulates
 * the Matrix multiplication done by a single thread given the input
 * Task t.
 *
 * @param t The Task passed as value that contains the information
 * about the corresponding block in Matrix C.
*/
void thread_mult(Task t) {

    // Matrices: A x B = C
    Matrix* A = t.A;
    Matrix* B_trans = t.B_trans;
    Matrix* C = t.C;

    // Extract Matrix dimensions
    size_t m = A->num_cols;
    size_t p = B_trans->num_rows;

    // retrieve internal Matrix arrays
    double* A_arr = A->values;
    double* B_trans_arr = B_trans->values;
    double* C_arr = C->values;

    // The block size to use (blocking method)
    size_t block_size = t.block_size;

    // Variables to describe the C block (start inclusive, end exclusive)
    size_t C_row_start = t.C_row_start;
    size_t C_col_start = t.C_col_start;
    size_t C_row_end = t.C_row_end;
    size_t C_col_end = t.C_col_end;

    // Loop goes through blocks in the shared dimension
    for (size_t k = 0; k < m; k += block_size) {
        size_t k_min = min(k + block_size, m);

        // These two loops let us consider a single element in C
        for (size_t ii = C_row_start; ii < C_row_end; ii++) {
            size_t a_row_offset = ii * m;
            for (size_t jj = C_col_start; jj < C_col_end; jj++) {

                // This loop handles the dot product (using loop unrolling)
                size_t kk = k;
                size_t c_index = ii * p + jj;
                size_t b_row_offset = jj * p;
                size_t p2 = p * 2;
                size_t p3 = p * 3;
                // Hint compiler to fetch from memory once
                double c_value = C_arr[c_index];
                for (; kk + 3 < k_min; kk += 4) {
                    // Updating a single element in C block
                    c_value += A_arr[a_row_offset + kk] * B_trans_arr[b_row_offset + kk];
                    c_value += A_arr[a_row_offset + (kk + 1)] * B_trans_arr[b_row_offset + p + kk];
                    c_value += A_arr[a_row_offset + (kk + 2)] * B_trans_arr[b_row_offset + p2 + kk];
                    c_value += A_arr[a_row_offset + (kk + 3)] * B_trans_arr[b_row_offset + p3 + kk];
                }
                // Handle residual operations not handled by the loop unrolling
                for (; kk < k_min; kk ++) {
                    // Updating a single element in C block
                    c_value += A_arr[a_row_offset + kk] * B_trans_arr[b_row_offset + kk];
                }
                // Write back to memory
                C_arr[c_index] = c_value;
            }
        }
    }
}

// Mutex lock used to access the Queue
pthread_mutex_t queue_lock;

/**
 * @brief Function used by the threads. A thread will access the Queue
 * and retrieve a Task object that describes a block of Matrix C that
 * needs to be calculated.
 *
 * @param A pointer to a ThreadArgs object on the heap that
 * contains the Queue.
 *
 * @return In both cases of success and failure, it returns NULL.
 * Failures are however logged using perror.
*/
void* process_tasks(void* arg) {

    // Extract argument
    Queue* q = (Queue*) arg;

    // Keep going until the Queue is empty (true due to mutex for Queue)
    while (true) {

        Task t;
        bool is_empty;

        // Lock the Queue with the mutex before accessing
        if (pthread_mutex_lock(&queue_lock) != 0) {
            perror("Error: Mutex lock failed");
            return NULL;
        }

        // Retrieve Queue data
        is_empty = queue_is_empty(q);
        if (!is_empty) {
            t = queue_get(q);
        }

        // Unlock the Queue
        if(pthread_mutex_unlock(&queue_lock) != 0) {
            perror("Error: Mutex unlock failed");
            return NULL;
        }

        if (is_empty) {
            // Queue is empty, unlock mutex and leave
            break;
        } else {
            // Perform Matrix multiplication with the Task
            thread_mult(t);
        }
    }

    return NULL;
}

Matrix* matrix_multithread_mult(Matrix* A, Matrix* B, size_t block_size, size_t NUM_THREADS) {

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

    size_t min_nm = min(n, m);
    if (block_size == 0) {
        errno = EINVAL;
        perror("Error: Block size cannot be of value 0");
        return NULL;
    }

    // Check if the block size needs to be adjusted for smaller matrices
    size_t smallest_dimension = min(min_nm, p);
    block_size = (block_size > smallest_dimension) ? smallest_dimension : block_size;

    // Instantiate Matrix C
    Matrix* C = matrix_create_with(pattern_zero, NULL, n, p);
    if (!C) {
        errno = EINVAL;
        perror("Error: Unable to allocate memory for Matrix C");
        return NULL;
    }

    // Create a Queue filled with all the tasks / blocks to calculate in C
    Queue* q = preprocessing(A, B, C, block_size);

    // Retrieve a pointer to B_transposed so that it can be later freed
    Matrix* B_trans = queue_peek(q).B_trans;

    // Initialize the mutex for the Queue
    pthread_mutex_init(&queue_lock, NULL);

    // Create array to hold threads
    pthread_t threads[NUM_THREADS];

    // Assign each thread to the task_worker() function
    for (size_t i = 0; i < NUM_THREADS; i++) {

        // Create a thread and check for successfull initialization
        if (pthread_create(&threads[i], NULL, process_tasks, q) != 0) {
            perror("Error: Creating thread failed");

            // Handle clean-up by canceling threads and freeing allocations
            for (size_t j = 0; j < i; j++) {
                if (pthread_cancel(threads[j]) != 0) {
                    perror("Error: Canceling thread failed");
                }
            }

            // Destory the Queue mutex
            if (pthread_mutex_destroy(&queue_lock) != 0) {
                perror("Error: Destorying queue_lock mutex failed");
            }

            return NULL;
        }
    }

    // Have the main thread wait for each thread to finish
    for (size_t i = 0; i < NUM_THREADS; i++) {

        if (pthread_join(threads[i], NULL) != 0) {
            perror("Error: pthread_join failed");

            free(q);
            pthread_mutex_destroy(&queue_lock);
            free(C);
            return NULL;
        }
    }

    // Free allocated memory
    free(q);
    free(B_trans);

    // Destory the Queue mutex
    pthread_mutex_destroy(&queue_lock);

    // Return the result
    return C;
}
