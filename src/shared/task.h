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

} Task;

#endif // TASK_H
