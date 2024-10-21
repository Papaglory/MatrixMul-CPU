#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "matrix.h"

Matrix* matrix_create_from_1D_array(size_t num_rows, size_t num_cols,
                                const double arr_values[num_rows * num_cols]) {

    if (num_rows == 0 || num_cols == 0) {
        errno = EINVAL;
        perror("Error: Both dimensions have to be greater than 0");
        return NULL;
    }

    // Allocate a 1D array with size determined by num_rows and num_cols
    double* values = (double*)malloc(sizeof(double) * num_rows * num_cols);
    if (!values) {
        errno = ENOMEM;
        perror("Error: Allocation of Matrix array failed");
        return NULL;
    }

    // Initialize row values according to input-array
    for (size_t i = 0; i < num_rows * num_cols; i++) {
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
    m->num_rows = num_rows;
    m->num_cols = num_cols;
    m->owns_rows = true;

    return m;
}

Matrix* matrix_create_from_2D_array(size_t num_rows, size_t num_cols,
                                const double arr_values[num_rows][num_cols]) {

    if (num_rows == 0 || num_cols == 0) {
        errno = EINVAL;
        perror("Error: Both dimensions have to be greater than 0");
        return NULL;
    }

    // Allocate a 1D array with size determined by num_rows and num_cols
    double* values = (double*)malloc(sizeof(double) * num_rows * num_cols);
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
    m->num_rows = num_rows;
    m->num_cols = num_cols;
    m->owns_rows = true;

    return m;
}

Matrix* matrix_create_from_pointers(size_t num_rows, size_t num_cols,
                                    double* values) {

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
    m->num_rows = num_rows;
    m->num_cols = num_cols;
    m->owns_rows = true;

    return m;
}

double* pattern_zero(double* values, void* args, size_t num) {

    if (!values) {
        errno = EINVAL;
        perror("Error: Argument pointer is NULL");
        return NULL;
    }
    return memset(values, 0, num * sizeof(double));
}

double* pattern_random_between(double* values, void* args, size_t num) {

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

Matrix* matrix_create_with(double* (*pattern)(double* values, void* args, size_t num), void* args, size_t num_rows, size_t num_cols) {

    if (num_rows == 0 || num_cols == 0 || !pattern) {
        errno = EINVAL;
        perror("Error: Invalid argument for matrix_generate_with()");
        return NULL;
    }

    // Allocate a 1D array with size determined by num_rows and num_cols
    double* values = (double*)calloc(num_rows * num_cols, sizeof(double));
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
    m->num_rows = num_rows;
    m->num_cols = num_cols;
    m->owns_rows = true;

    return m;
}

/**
 * @brief A helper function to insert a single escape sequence into buffer.
 *
 * @param val The escape sequence to be inserted.
 * @param buffer The buffer to insert the escape sequence into.
 * @param buffer_index A pointer to represent the address in buffer where
 * we want to insert the escape sequence.
 * @param buffer_size The total size in bytes of the buffer.
 *
 * @return The buffer_index incremented by a byte (for the escape sequence).
 */
char* insert_escape_sequence(char val, char* buffer, char* buffer_index, size_t buffer_size) {

    if (!buffer || !buffer_index) {
        errno = EINVAL;
        perror("Error: Invalid arguments for insert_escape_sequence()");
        return NULL;
    }

    /*
     * Check for buffer overflow.
     * If it is the null terminator, we do not check for
     * equality as the null terminator is the final byte.
     */
    if (val == '\0') {
        if ((buffer_index - buffer) + 1 > buffer_size) {
            // Writing into buffer will result in overflow
            errno = ENOMEM;
            perror("Error: Buffer overflow prevented. Function returned");
            return NULL;
        }
    } else {
        if ((buffer_index - buffer) + 1 >= buffer_size) {
            // Writing into buffer will result in overflow
            errno = ENOMEM;
            perror("Error: Buffer overflow prevented. Function returned");
            return NULL;
        }
    }

    // Add the escape sequence
    int index = buffer_index - buffer;
    buffer[index] = val;
    // Move buffer_index forward by the byte for the escape sequence
    return ++buffer_index;
}

int matrix_print(Matrix* m) {

    if (!m || !m->values) {
        errno = EINVAL;
        perror("Error: There are missing arguments");
        return -1;
    }

    // Parameter to determine how many decimal places to show
    const int FLOAT_PRECISION = 2;
    // Parameter to have a maximum before turning into scientific notation
    const int FLOAT_MAX = 100000;
    // A safety margin to prevent buffer overflow (in case of unpredicted Matrix cases)
    const double BUFFER_SAFETY_MARGIN = 1.1f;

    /*
    */

    // Allocate the buffer on the heap (+1 for the null terminator)
    size_t buffer_size = (m->num_rows * m->num_cols) * (9 + FLOAT_PRECISION) + m->num_rows + 1;
    // Apply the safety margin
    buffer_size = (size_t) buffer_size * BUFFER_SAFETY_MARGIN;
    char* buffer = (char*)malloc(buffer_size);
    if (!buffer) {
        errno = ENOMEM;
        perror("Error: Failed to allocate print buffer");
        return -1;
    }

    // Add Matrix elements to the buffer
    char* buffer_index = &buffer[0];
    for (size_t i = 0; i < m->num_rows; i++) {
        for (size_t j = 0; j < m->num_cols; j++) {

            // Retrieve Matrix element
            double val = m->values[i * m->num_cols + j];
            // Auxiliary buffer to place element into (+1 for null terminator)
            char temp[12];

            // Determine whether to use scientific or "normal" notation
            if (val < -FLOAT_MAX || val  > FLOAT_MAX) {
                // Scientific notation
                snprintf(temp, sizeof(temp), "%10.*e ", FLOAT_PRECISION, val);
            } else {
                // Normal notation
                snprintf(temp, sizeof(temp), "%10.*f ", FLOAT_PRECISION, val);
            }

            /*
             * Before writing to buffer, we make sure no
             * buffer overflow occurs. We check the number of
             * bytes used after write does not exceed the total
             * bytes in the buffer. Equal is used because we want
             * space for the final null terminator at the end.
            */
            size_t strlen_temp = strlen(temp);
            if ((buffer_index - buffer) + strlen_temp >= buffer_size) {
                // Writing into buffer will result in overflow
                errno = ENOMEM;
                perror("Error: Buffer overflow prevented. Function returned");
                free(buffer);
                return -1;
            }
            memcpy(buffer_index, temp, strlen_temp);
            // Move buffer_index forward by the number of bytes copied from temp
            buffer_index += strlen_temp;
        }

        // Insert an end-of-line character at the end of the row
        buffer_index = insert_escape_sequence('\n', buffer, buffer_index, buffer_size);
        if (!buffer_index) {
            // Insert escape sequence failed
            free(buffer);
            return -1;
        }
    }

    // Insert a null terminator at the end of the buffer (string)
    buffer_index = insert_escape_sequence('\0', buffer, buffer_index, buffer_size);
    if (!buffer_index) {
        // Insert escape sequence failed
        free(buffer);
        return -1;
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

    if (m->owns_rows) {
        free(m->values);
    }
    free(m);

    return 0;
}
