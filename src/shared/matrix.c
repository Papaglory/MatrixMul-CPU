#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "matrix.h"

Matrix* matrix_create_from_1D_array(size_t num_rows, size_t num_cols,
                                const int arr_values[num_rows * num_cols]) {

    if (num_rows == 0 || num_cols == 0) {
        errno = EINVAL;
        perror("Error: Both dimensions have to be greater than 0");
        return NULL;
    }

    // Allocate a 1D array with size determined by num_rows and num_cols
    int* values = (int*)malloc(sizeof(int) * num_rows * num_cols);
    if (!values) {
        errno = ENOMEM;
        perror("Error: Allocation of Matrix array failed");
        return NULL;
    }

    // Initialize row values according to input-array
    for (int i = 0; i < num_rows * num_cols; i++) {
        values[i] = arr_values[i];
    }

    // Create the Matrix
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    if (!m) {
        // free allocated matrix array elements before returning
        free(values);
        errno = ENOMEM;
        perror("Error: Allocation of the Matrix failed");
        return NULL;
    }

    // Set Matrix member variables
    m->values = values;
    *((int*)m->num_rows) = num_rows;
    *((int*)m->num_cols) = num_cols;
    *((int*)m->owns_rows) = true;

    return m;
}

Matrix* matrix_create_from_2D_array(size_t num_rows, size_t num_cols,
                                const int arr_values[num_rows][num_cols]) {

    if (num_rows == 0 || num_cols == 0) {
        errno = EINVAL;
        perror("Error: Both dimensions have to be greater than 0");
        return NULL;
    }

    // Allocate a 1D array with size determined by num_rows and num_cols
    int* values = (int*)malloc(sizeof(int) * num_rows * num_cols);
    if (!values) {
        errno = ENOMEM;
        perror("Error: Allocation of Matrix array failed");
        return NULL;
    }

    // Initialize row values according to input-array
    for (size_t i = 0; i < num_rows; i++) {
        for (size_t j = 0; j < num_cols; j++) {
            values[i * num_cols + j] = arr_values[i][j];
        }
    }

    // Create the Matrix
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    if (!m) {
        // free allocated matrix array elements before returning
        free(values);
        errno = ENOMEM;
        perror("Error: Allocation of the Matrix failed");
        return NULL;
    }

    // Set Matrix member variables
    m->values = values;
    *((int*)m->num_rows) = num_rows;
    *((int*)m->num_cols) = num_cols;
    *((int*)m->owns_rows) = true;

    return m;
}

Matrix* matrix_create_from_pointers(size_t num_rows, size_t num_cols,
                                    int* values) {

    if (num_rows == 0 || num_cols == 0 || !values) {
        errno = EINVAL;
        perror("Error: Either non-zero dimensions or invalid 'rows' pointer");
        return NULL;
    }

    // Create the Matrix
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    if (!m) {
        // Matrix allocation failed
        errno = ENOMEM;
        perror("Error: Failed to allocate Matrix");
        return NULL;
    }

    // Set Matrix member variables
    m->values = values;
    *((int*)m->num_rows) = num_rows;
    *((int*)m->num_cols) = num_cols;
    *((int*)m->owns_rows) = true;

    return m;
}

int* pattern_zero(int* values, void* args, size_t num) {

    if (!values) {
        errno = EINVAL;
        perror("Error: Argument pointer is NULL");
        return NULL;
    }
    return memset(values, 0, num * sizeof(int));
}

int* pattern_random_between(int* values, void* args, size_t num) {

    if (!values || !args) {
        errno = EINVAL;
        perror("Error: Argument pointer is NULL");
        return NULL;
    }

    // Retrieve arguments
    int* int_args = (int*)args;
    int min = int_args[0];
    int max = int_args[1];
    if (min > max) {
        errno = EINVAL;
        perror("Error: min is greater than max");
        return NULL;
    }

    // Initialize row elements
    for (size_t i = 0; i < num; i++) {
        values[i] = rand() % (max - min + 1) + min;
        //TODO Consider switching rand for multithread.
    }

    return values;
}

Matrix* matrix_create_with(int* (*pattern)(int* values, void* args, size_t num), void* args, size_t num_rows, size_t num_cols) {

    if (num_rows == 0 || num_cols == 0 || !pattern) {
        errno = EINVAL;
        perror("Error: Invalid argument for matrix_generate_with()");
        return NULL;
    }

    // Allocate a 1D array with size determined by num_rows and num_cols
    int* values = (int*)calloc(num_rows * num_cols, sizeof(int));
    if (!values) {
        errno = EINVAL;
        perror("Error: Allocation failed for Matrix array");
        return NULL;
    }

    // Apply the pattern to the whole array
    values = pattern(values, args, num_rows * num_cols);
    if (!values) {
        // Something went wrong
        return NULL;
    }

    // Create the Matrix
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    if (!m) {
        // free allocated matrix array values before returning
        free(values);
        errno = ENOMEM;
        perror("Error: Allocation of the Matrix failed");
        return NULL;
    }

    // Set Matrix member variables
    m->values = values;
    *((int*)m->num_rows) = num_rows;
    *((int*)m->num_cols) = num_cols;
    *((int*)m->owns_rows) = true;

    return m;
}

int matrix_print(Matrix* m) {

    if (!m || !m->values) {
        errno = EINVAL;
        perror("Error: There are missing arguments");
        return -1;
    }

    // Allocate the buffer on the heap (+1 for the null terminator)
    size_t buffer_size = (m->num_rows * m->num_cols) * 11 + m->num_rows + 1;
    char* buffer = (char*)malloc(buffer_size);
    if (!buffer) {
        errno = ENOMEM;
        perror("Error: Failed to allocate print buffer");
        return -1;
    }

    // Set the buffer to the empty string
    buffer[0] = '\0';

    // Add Matrix elements to the buffer
    for (size_t i = 0; i < m->num_rows; i++) {
        for (size_t j = 0; j < m->num_cols; j++) {

            int val = m->values[i * m->num_cols + j];
            char temp[12]; // +1 for null terminator
            snprintf(temp, sizeof(temp), "%10d ", val);
            strncat(buffer, temp, buffer_size - strlen(buffer) - 1);
        }

        // Add a newline at the end of the row
        strncat(buffer, "\n", buffer_size - strlen(buffer) - 1);
    }

    // Print the entire buffer at once
    printf("%s", buffer);

    free(buffer);
    return 0;
}

int matrix_free(Matrix* m) {

    if (!m) {
        errno = EINVAL;
        perror("Error: There is no Matrix to free");
        return -1;
    }

    free(m->values);
    free(m);

    return 0;
}
