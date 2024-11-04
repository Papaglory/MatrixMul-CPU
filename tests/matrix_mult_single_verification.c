#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../src/shared/matrix.h"
#include "../src/cpu/matrix_singlethread.h"
#include "../src/shared/matrix_utils.h"

int main() {

    printf("%s\n", "--------STARTING matrix_mult_single_verification.c--------");

    // Benchmark parameters
    const int RUN_COUNT = 10;
    const int BLOCK_SIZE = 1; // Does not matter since we only care about result
    // Used if there are different rounding errors between the implementations
    const double APPROXIMATION_THRESHOLD = 1e-9;

    // Matrix generation parameters
    const double VALUES_MIN = -1e-9;
    const double VALUES_MAX = 1e-9;
    const size_t DIMENSIONS_MIN = 1000;
    const size_t DIMENSIONS_MAX = 1000;
    const int seed = 42;

    // Set the seed for reproducibility
    srand(seed);

    bool mismatch_detected = false;
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
        Matrix* C = matrix_singlethread_mult(A, B, 1);
        matrix_mult_openblas(A->values, B->values, C_blas, n, m, p);

        // Compare result
        for (size_t j = 0; j < n * p; j++) {

            if (fabs(C->values[j] - C_blas[j]) > APPROXIMATION_THRESHOLD) {
                printf("Error: The matrix mult result differs!\n");
                mismatch_detected = true;
                break;
            }
        }

        Matrix* C_m = matrix_create_with(pattern_zero, NULL, n, p);
        C_m->values = C_blas;

        // If there is a mismatch, inspect the two matrices
        if (mismatch_detected) {
            printf("My Matrix\n");
            matrix_print(C);
            printf("BLAS Matrix\n");
            matrix_print(C_m);

            // Free the allocated data corresponding to this run
            matrix_free(A);
            matrix_free(B);
            matrix_free(C);
            free(C_blas);

            return 0;
        }

        // Free the allocated data corresponding to this run
        matrix_free(A);
        matrix_free(B);
        matrix_free(C);
        free(C_blas);
    }

    printf("%s\n", "All calculations are correct");
    printf("%s\n", "--------FINISHED matrix_mult_single_verification.c--------");

    return 0;
}
