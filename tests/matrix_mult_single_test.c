#include "../src/shared/matrix.h"
#include "../src/cpu/matrix_singlethread.h"
#include <stdio.h>
#include <stdlib.h>

int main() {

    printf("%s\n", "--------STARTING matrix_mult_single_test.c--------");

    printf("%s\n", "Creating a Matrix from array on the stack");
    int arr[2][2] = {
        {1,2},
        {12,4}
    };
    Matrix* m2 = matrix_create_from_2D_array(2, 2, arr);
    matrix_print(m2);



    return 0;
}
