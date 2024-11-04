#include "../../src/shared/matrix.h"
#include <stdio.h>
#include <stdlib.h>

int main() {

    printf("%s\n\n", "--------STARTING matrix_init_test.c--------");

    printf("%s\n", "Creating a zero Matrix");
    Matrix* m1 = matrix_create_with(pattern_zero, NULL, 3, 2);
    matrix_print(m1);

    printf("%s\n", "Creating a random Matrix");
    int* rand_args = (int*)malloc(sizeof(int) * 2);
    rand_args[0] = 10;
    rand_args[1] = 20;
    Matrix* m0 = matrix_create_with(pattern_random_between, rand_args, 3, 2);
    matrix_print(m0);

    printf("%s\n", "Creating a Matrix from array on the stack");
    double arr[2][3] = {
        {1,2,3},
        {12,8,4}
    };
    Matrix* m2 = matrix_create_from_2D_array(2, 3, arr);
    matrix_print(m2);

    printf("%s\n", "Creating a Matrix from array on the heap");
    size_t num_rows = 2;
    size_t num_cols = 2;
    double* values = (double*)malloc(sizeof(double) * num_rows * num_cols);
    if (!values) {
        printf("Heap allocation failed!");
    }

    // Set the Matrix values
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            values[i * num_rows + j] = i * j + 1;
        }
    }

    Matrix* m3 = matrix_create_from_pointers(num_rows, num_cols, values);
    matrix_print(m3);

    return 0;
}
