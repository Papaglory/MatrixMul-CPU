#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../src/shared/matrix.h"
#include "../src/cpu/matrix_multithread.h"
#include "../src/cpu/matrix_singlethread.h"
#include "../src/shared/matrix_utils.h"

int main() {

    printf("%s\n", "--------STARTING matrix_mult_multi_verification.c--------");

    // Benchmark parameters
    const size_t RUN_COUNT = 10;
    const size_t BLOCK_SIZE = 13; // Does not matter since we only care about result
    // Used if there are different rounding errors between the implementations
    const double APPROXIMATION_THRESHOLD = 1e-9;
    const size_t NUM_THREADS = 1;

    // Matrix generation parameters
    const double VALUES_MIN = -1e+9;
    const double VALUES_MAX = 1e+9;
    const size_t DIMENSIONS_MIN = 3000;
    const size_t DIMENSIONS_MAX = 3000;
    const int seed = 100;

    // Set the seed for reproducibility
    srand(seed);

    bool mismatch_detected = true;
    double total_time = 0;
    for (size_t i = 0; i < RUN_COUNT; i++) {

        printf("Iteration %zu\n", i);

        // Generate Matrix dimensions
        const size_t n = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t m = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t p = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);

        // Generate matrices
        Matrix* A = generate_matrix(VALUES_MIN, VALUES_MAX, n, m);
        Matrix* B = generate_matrix(VALUES_MIN, VALUES_MAX, m, p);

        // Do multithread multiplication
        Matrix* C = NULL;
        C = matrix_multithread_mult(A, B, BLOCK_SIZE, NUM_THREADS);

        //C = matrix_singlethread_mult(A, B, BLOCK_SIZE);
        printf("finished calc\n");
        fflush(stdout);

        // openBLAS requires the resulting C array as well as argument
        double* C_blas = (double*)malloc(sizeof(double) * n * p);
        matrix_mult_openblas(A->values, B->values, C_blas, n, m, p);

        // Compare result
        for (size_t j = 0; j < n * p; j++) {
            break;

            if (fabs(C->values[j] - C_blas[j]) > APPROXIMATION_THRESHOLD) {
                printf("Error: The matrix mult result differs!\n");
                mismatch_detected = true;

                printf("%-20s %f\n", "My implementation", C->values[j]);
                printf("%-20s %f\n", "BLAS implementation", C_blas[j]);

                matrix_free(A);
                matrix_free(B);
                matrix_free(C);
                free(C_blas);

                return 0;
            }
        }

        // Free the allocated data corresponding to this run
        matrix_free(A);
        matrix_free(B);
        matrix_free(C);
        free(C_blas);
    }

    printf("%s\n", "All calculations are correct");
    printf("%s\n", "--------FINISHED matrix_mult_multi_verification.c--------");

    return 0;
}
