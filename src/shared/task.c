#include "task.h"
#include <errno.h>
#include <stdio.h>

Task task_create(Matrix* A, Matrix* B_trans, Matrix* C, size_t block_size,
                 size_t C_row_start, size_t C_col_start,
                 size_t C_row_end, size_t C_col_end) {

    if (!A || !B_trans || !C) {

        errno = EINVAL;
        perror("Error: Some of the matrices are missing");
        Task empty_task = {0};
        return empty_task;
    }

    // Create task and member variables
    Task t;
    t.A = A;
    t.B_trans = B_trans;
    t.C = C;
    t.block_size = block_size;
    t.C_row_start = C_row_start;
    t.C_col_start = C_col_start;
    t.C_row_end = C_row_end;
    t.C_col_end = C_col_end;
    t.is_valid = true;

    return t;
}
