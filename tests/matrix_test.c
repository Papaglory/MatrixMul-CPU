#include "../src/shared/matrix.h"
#include <stdio.h>
#include <stdlib.h>

int main() {

    printf("%s\n", "Creating a zero Matrix");
    Matrix* m1 = matrix_create_zero(3, 3);
    matrix_print(m1);

    printf("%s\n", "Creating a Matrix from array on the stack");
    int arr[2][3] = {
        {1,2,3},
        {11,8,4}
    };
    Matrix* m2 = matrix_create_from_array(2, 3, arr);
    matrix_print(m2);

    printf("%s\n", "Creating a Matrix from array on the heap");
    size_t num_rows = 2;
    size_t num_cols = 2;
    int** heap_arr = (int**)malloc(sizeof(int*) * num_rows);
    if (!heap_arr) {
        printf("Heap allocation failed!");
    }
    for (int row = 0; row < num_rows; row++) {
        heap_arr[row] = (int*)malloc(sizeof(int) * num_cols);
        if (!heap_arr[row]) {
            printf("Heap allocation failed!");
        }
        for (int col = 0; col < num_cols; col++) {
            heap_arr[row][col] = row * col + 1;
        }
    }

    Matrix* m3 = matrix_create_from_pointers(num_rows, num_cols, heap_arr);
    matrix_print(m3);

    return 0;
}
