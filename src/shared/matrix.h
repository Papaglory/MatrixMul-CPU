#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

typedef struct {

    /*
     * A pointer pointing to a pointer that points to an integer.
     * The idea of an array on the heap is handled through the
     * use of pointers.
    */
    int** rows;

    size_t num_rows;
    size_t num_cols;

} matrix;

matrix* matrix_create();

matrix* matrix_add(void* matrix_1, void* matrix_2);

matrix* matrix_mult(void* matrix_1, void* matrix_2);

#endif // MATRIX_H
