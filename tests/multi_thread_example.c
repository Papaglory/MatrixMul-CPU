#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "../src/shared/queue.h"

typedef struct {

    Queue* q;
    size_t FINAL_VALUE;
    double* arr;

} ThreadArgs;

pthread_mutex_t q_lock;

void* worker_thread(void* arg) {

    // Unpack arguments
    ThreadArgs* args = arg;
    Queue* q = args->q;
    size_t FINAL_VALUE = args->FINAL_VALUE;
    double* arr = args->arr;

    while (true) {

        // Interact with the Queue
        pthread_mutex_lock(&q_lock);
        if (queue_is_empty(q)) {

            pthread_mutex_unlock(&q_lock);
            break;
        }

        // Retrieve the task
        Task t = queue_get(q);

        // Release the mutex
        pthread_mutex_unlock(&q_lock);

        // Process the task
        size_t index = t.block_size;
        while (arr[index] < FINAL_VALUE) {
            arr[index] *= 1.000001;  // Small multiplication to simulate more intensive computation
        }
    }

    return NULL;
}

int main() {

    printf("%s\n", "Starting multi_thread_example.c");

    // Utils for tracking time
    struct timespec start, end;
    double elapsed_time;

    // Create Queue
    const size_t NUM_TASKS = 100;
    Queue* q = queue_create(NUM_TASKS);

    // Enqueue tasks
    for (size_t i = 0; i < NUM_TASKS; i++) {
        Task t = {0};
        t.block_size = i;
        queue_add(q, t);
    }

    // Create array threads will work with
    double* arr = (double*)calloc(NUM_TASKS, sizeof(double));
    if (!arr) {
        perror("Error: arr allocation failed");
    }
    for (size_t i = 0; i < NUM_TASKS; i++) {
        arr[i] = 1;
    }

    // Allocate threads
    const size_t NUM_THREADS = 1;
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
    if (!threads) {
        perror("Error: threads allocation failed");
    }

    // The value each array element will have at the end
    const size_t FINAL_VALUE = 1000000000;

    // Create the arguments for the thread function
    ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
    if (!args) {
        perror("Error: Args allocation failed");
    }
    args->q = q;
    args->FINAL_VALUE = FINAL_VALUE;
    args->arr = arr;

    // Start timer
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Initialize the mutex for the Queue
    pthread_mutex_init(&q_lock, NULL);

    // Create threads
    for (size_t i = 0; i < NUM_THREADS; i++) {

        if (pthread_create(&threads[i], NULL, worker_thread, args) != 0) {
            perror("Error: Creating thread failed");
            for (size_t j = 0; j < i; j++) {
                pthread_cancel(threads[j]);
            }
        }
    }

    // Wait for threads to finish
    for (size_t i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // End timer
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Check each value is correct
    const double EPSILON = 1e-6;
    for (size_t i = 0; i < NUM_TASKS; i ++) {
        if (arr[i] < FINAL_VALUE) {
            printf("Wrong array value at index %zu\n", i);
        }
    }

    for (size_t i = 0; i < NUM_TASKS; i++) {
        if (arr[i] < FINAL_VALUE) {
            perror("ERRO");
            break;
        }
    }

    // Calculate elapsed time in seconds
    elapsed_time = (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%s %f\n", "Elapsed time:", elapsed_time);

    pthread_mutex_destroy(&q_lock);
    free(arr);
    free(threads);
    free(args);

    printf("%s\n", "Finishing multi_thread_example.c");

    return 0;
}
