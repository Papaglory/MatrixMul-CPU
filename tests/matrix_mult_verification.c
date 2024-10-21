#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../src/shared/matrix.h"
#include "../src/cpu/matrix_singlethread.h"
#include "../src/shared/matrix_utils.h"

int main() {

    printf("%s\n", "--------STARTING matrix_mult_verification.c--------");

    // Benchmark parameters
    const int RUN_COUNT = 1;
    const int BLOCK_SIZE = 1; // Does not matter since we only care about result
    // If there are different rounding approximations between the implementations
    const double APPROXIMATION_THRESHOLD = 0.1f;

    // Matrix generation parameters
    const int VALUES_MIN = -1000;
    const int VALUES_MAX = 1000;
    const size_t DIMENSIONS_MIN = 1;
    const size_t DIMENSIONS_MAX = 10;
    const int seed = 42;

    struct timespec start, end;
    double elapsed_time;

    // Set the seed for reproducibility
    srand(seed);

    double total_time = 0;
    for (size_t i = 0; i < RUN_COUNT; i++) {

        // Generate Matrix dimensions
        const size_t n = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t m = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t p = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);

        // Generate matrices
        Matrix* A = generate_matrix(VALUES_MIN, VALUES_MAX, n, m);
        Matrix* B = generate_matrix(VALUES_MIN, VALUES_MAX, m, p);

        // openBLAS requires the resulting C array as well as argument
        double* C_blas = (double*)malloc(sizeof(double) * n * p);

        // Do the matrix multiplications
        Matrix* C = matrix_singlethread_mult(A, B, 2);
        matrix_mult_openblas(A->values, B->values, C_blas, n, m, p);

        // Compare result
        for (size_t j = 0; j < n * p; j++) {

            if (fabs(C->values[j] - C_blas[j]) > APPROXIMATION_THRESHOLD) {
                printf("Error: The matrix mult result differs!\n");
                break;
            }
        }

        Matrix* C_m = matrix_create_with(pattern_zero, NULL, n, p);
        C_m->values = C_blas;

        printf("My Matrix\n");
        matrix_print(C);
        printf("BLAS Matrix\n");
        matrix_print(C_m);
    }

    return 0;
}
