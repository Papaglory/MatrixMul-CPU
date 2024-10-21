#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../src/shared/matrix.h"
#include "../src/cpu/matrix_singlethread.h"
#include "../src/shared/matrix_utils.h"

void benchmark_test() {

    // Benchmark parameters
    const int RUN_COUNT = 1;
    const int WARMUP_COUNT = 1;
    const int AVERAGE_COUNT = 1;

    // Matrix generation parameters
    const int VALUES_MIN = -1000;
    const int VALUES_MAX = 1000;
    const size_t DIMENSIONS_MIN = 1;
    const size_t DIMENSIONS_MAX = 10;

    struct timespec start, end;
    double elapsed_time;

    double total_time = 0;
    for (size_t i = 0; i < RUN_COUNT; i++) {

        // Generate Matrix dimensions
        const size_t n = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t m = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);
        const size_t p = random_between(DIMENSIONS_MIN, DIMENSIONS_MAX);

        // Generate matrices
        Matrix* A = generate_matrix(VALUES_MIN, VALUES_MAX, n, m);
        Matrix* B = generate_matrix(VALUES_MIN, VALUES_MAX, m, p);


        // Perform warmup
        for (size_t j = 0; j < WARMUP_COUNT; j++) {

            //mult(A, B);
        }

        double time = 0;
        for (size_t j = 0; j < AVERAGE_COUNT; j++) {

            clock_gettime(CLOCK_MONOTONIC, &start);
            //mult(A, B);
            clock_gettime(CLOCK_MONOTONIC, &end);

            // Calculate elapsed time in seconds
            elapsed_time = (end.tv_sec - start.tv_sec) +
                           (end.tv_nsec - start.tv_nsec) / 1e9;
            time += elapsed_time;
        }

        total_time += time / AVERAGE_COUNT;
    }
}
