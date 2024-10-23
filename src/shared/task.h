#ifndef TASK_H
#define TASK_H

#include "matrix.h"

typedef struct {

    // The matrices corresopnding to A x B = C.
    Matrix* A;
    Matrix* B;
    Matrix* C;

    // The block size to use (blocking method)
    size_t block_size;

    // Variables to describe the C block (start inclusive, end exclusive)
    size_t C_row_start;
    size_t C_col_start;
    size_t C_row_end;
    size_t C_col_end;

    // Validity variable: true = "valid Task" and false = empty or invalid task
    bool is_valid;

} Task;

/**
 * @brief Create a Task object for matrices A x B  = C.
 *
 * @param A Matrix pointer.
 * @param B Matrix pointer.
 * @param C Matrix pointer.
 * @param block_size The size used in the blocking method.
 * @param c_row_start The row that represents the start of the row block in C.
 * @param c_col_start The col that represents the start of the col block in C.
 * @param c_row_end The row that represents the end of the row block in C.
 * @param c_col_end The col that represents the end of the col block in C.
 *
 * @return The Task passed as value.
*/
Task task_create(Matrix* A, Matrix* B, Matrix* C, size_t block_size,
                 size_t C_row_start, size_t C_col_start,
                 size_t C_row_end, size_t C_col_end);

#endif // TASK_H
