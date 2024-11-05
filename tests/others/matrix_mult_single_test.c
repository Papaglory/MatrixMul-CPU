#include "../../src/shared/matrix.h"
#include "../../src/cpu/matrix_singlethread.h"
#include <stdio.h>

int main() {

    printf("%s\n", "--------STARTING matrix_mult_single_test.c--------");

    printf("%s\n", "Creating a Matrix from array on the stack");
    double arr1[3][3] = {
        {1,3,2},
        {5,3,2},
        {1,0,1}
    };
    Matrix* m1 = matrix_create_from_2D_array(3, 3, arr1);
    printf("%s\n", "Matrix1");
    matrix_print(m1);

    double arr2[3][2] = {
        {1,0},
        {0,1},
        {0,0}
    };
    printf("%s\n", "Matrix2");
    Matrix* m2 = matrix_create_from_2D_array(3, 2, arr2);
    matrix_print(m2);

    // Allocate result Matrix
    Matrix* result = matrix_create_with(pattern_zero, NULL, 3, 2);

    size_t block_size = 1;
    printf("%s\n", "Calling matrix mult");
    matrix_singlethread_mult(m1, m2, result, block_size);
    matrix_print(result);

    return 0;
}
