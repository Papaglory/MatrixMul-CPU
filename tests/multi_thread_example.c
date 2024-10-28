/**
 * @file multi_thread_example.c
 *
 * @brief
 * This file is used as a simple proving ground for utilizing multiple
 * threads. The scenario is constructed to simmulate the environment
 * found in matrix_multithread.c where we have a main thread that creates
 * threads that retrieves work from a Queue which corresponds to blocks
 * found in the Matrix that needs to be calculated.
 */

#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../src/shared/queue.h"

typedef struct {

    Queue* q;
    size_t FINAL_VALUE;
    double* arr;
    size_t batch_size;

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
        if (pthread_mutex_lock(&q_lock) != 0) {
            printf("Something went wrong with locking\n");
        }
        if (queue_is_empty(q)) {

            if (pthread_mutex_unlock(&q_lock) != 0) {
                printf("Something went wrong with unlocking\n");
            }
            break;
        }

        // Retrieve the task
        Task t = queue_get(q);

        // Release the mutex
        if (pthread_mutex_unlock(&q_lock) != 0) {
            printf("Something went wrong with unlocking\n");
        }

        // Process the task
        size_t index = t.block_size;
        while (arr[index] < FINAL_VALUE) {
            arr[index] *= 1.0000001;  // Small multiplication to simulate more intensive computation
        }
    }

    return NULL;
}

/**
 * @brief A simple multithread example using a standard queue to hand out
 * tasks for the threads. Each Task corresponds with increasing a double
 * element within an array as long as it is smaller than FINAL_VALUE.
 *
 * @param NUM_TASKS The number of tasks in the Queue.
 * @param NUM_THREADS The number of threads working concurrently.
 * @param FINAL_VALUE The final value that acts as part of the condition
 * check when incrementing the values in the array.
 *
 * @return A value indicating the time to solve the problem. The timer starts
 * immediately after function call and includes both preprocessing and
 * clean-up. Returns -1 if error occured.
 */
double test_single_queue(size_t NUM_TASKS, size_t NUM_THREADS, size_t FINAL_VALUE) {

    // Utils for tracking time
    struct timespec start, end;
    double elapsed_time;

    // Start timer
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create Queue
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
        return -1;
    }
    for (size_t i = 0; i < NUM_TASKS; i++) {
        arr[i] = 1;
    }

    // Allocate threads
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
    if (!threads) {
        perror("Error: threads allocation failed");
        return -1;
    }

    // Create the arguments for the thread function
    ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
    if (!args) {
        perror("Error: Args allocation failed");
        return -1;
    }
    args->q = q;
    args->FINAL_VALUE = FINAL_VALUE;
    args->arr = arr;

    // Initialize the mutex for the Queue
    if (pthread_mutex_init(&q_lock, NULL) != 0) {
        printf("Something went wrong with mutex init\n");
    }

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
        if (pthread_join(threads[i], NULL) != 0) {
            printf("Something went wrong with ptrhead_join\n");
        }
    }

    // Check each value is correct
    for (size_t i = 0; i < NUM_TASKS; i ++) {
        if (arr[i] < FINAL_VALUE) {
            printf("Wrong array value at index %zu\n", i);
        }
    }

    // Destory the Queue mutex
    if (pthread_mutex_destroy(&q_lock) != 0) {
        printf("Something went wrong with destorying mutex\n");
    }

    // Clean up heap
    free(arr);
    free(threads);
    free(args);

    // End timer
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate elapsed time in seconds
    elapsed_time = (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%-35s %f\n", "Elapsed time:", elapsed_time);

    return elapsed_time;
}

/**
 * TODO: Does not use a mutex as the threads interact with their own Queue.
 */
void* worker_thread_queues(void* arg) {

    // Unpack arguments
    ThreadArgs* args = arg;
    Queue* q = args->q;
    size_t FINAL_VALUE = args->FINAL_VALUE;
    double* arr = args->arr;

    while (!queue_is_empty(q)) {

        // Retrieve the task
        Task t = queue_get(q);

        // Process the task
        size_t index = t.block_size;
        while (arr[index] < FINAL_VALUE) {
            arr[index] *= 1.0000001;  // Small multiplication to simulate more intensive computation
        }
    }

    return NULL;
}

/**
 * TODO: Does not use a mutex as the threads interact with their own Queue.
 * Same function but uses batches instead of retrieving single Task.
 */
void* worker_thread_queues_batch(void* arg) {

    // Unpack arguments
    ThreadArgs* args = arg;
    Queue* q = args->q;
    size_t FINAL_VALUE = args->FINAL_VALUE;
    double* arr = args->arr;

    while (!queue_is_empty(q)) {

        // Retrieve batch of tasks
        size_t batch_size = args->batch_size;
        Task* batch = queue_get_batch(q, &batch_size);

        // Process each Task in the batch
        for (size_t i = 0; i < batch_size; i++) {
            Task t = batch[i];
            size_t index = t.block_size;
            while (arr[index] < FINAL_VALUE) {
                arr[index] *= 1.0000001;  // Small multiplication to simulate more intensive computation
            }
        }
        free(batch);
    }

    return NULL;
}

/**
 * @brief A simple multithread example having a unique Queue for each
 * thread where the corresponding thread can retrieve a task.
 * Each Task corresponds with increasing a double
 * element within an array as long as it is smaller than FINAL_VALUE.
 *
 * @details This implementation corresponds with dividing the workload
 * beforehand into each Queue.
 *
 * @param NUM_TASKS The number of tasks in the Queue.
 * @param NUM_THREADS The number of threads working concurrently.
 * @param FINAL_VALUE The final value that acts as part of the condition
 * check when incrementing the values in the array.
 * @param BATCH_SIZE The batch size used when a thread retrieves
 * tasks from the Queue.
 *
 * @return A value indicating the time to solve the problem. The timer starts
 * immediately after function call and includes both preprocessing and
 * clean-up. Returns -1 if error occured.
 */
double test_local_queues(size_t NUM_TASKS, size_t NUM_THREADS, size_t FINAL_VALUE, size_t BATCH_SIZE) {

    if (NUM_TASKS == 0 || NUM_THREADS == 0 || FINAL_VALUE == 0) {
        errno = EINVAL;
        perror("Error: Invalid arguments\n");
    }

    // Utils for tracking time
    struct timespec start, end;
    double elapsed_time;

    // Start timer
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create array threads will work on
    double* arr = (double*)calloc(NUM_TASKS, sizeof(double));
    if (!arr) {
        perror("Error: arr allocation failed");
        return -1;
    }
    // Initialize to 1 since threads will do *= operation
    for (size_t i = 0; i < NUM_TASKS; i++) {
        arr[i] = 1;
    }

    // Create an array of queues for each thread
    Queue** queues = (Queue**)malloc(sizeof(Queue*) * NUM_THREADS);
    if (!queues) {
        perror("Error: Allocation of queues failed\n");
        return -1;
    }

    // Enqueue tasks equally into each Queue
    const size_t TASKS_PER_QUEUE = NUM_TASKS / NUM_THREADS;
    size_t residual_tasks = NUM_TASKS % NUM_THREADS;
    printf("%s %zu\n", "Tasks per queue:", TASKS_PER_QUEUE);
    printf("%s %zu\n", "residual_tasks:", residual_tasks);
    size_t tasks_created = 0; // Is used to keep track of task / array element
    for (size_t i = 0; i < NUM_THREADS; i++) {

        // Create Queue
        if (residual_tasks > 0) {
            // Create Queue with room for one more task
            queues[i] = queue_create(TASKS_PER_QUEUE + 1);
            // Decrease by 1 since this Queue deals with 1 extra Task
            residual_tasks--;
        } else {
            // Residual tasks have been handled, can create normal Queue
            queues[i] = queue_create(TASKS_PER_QUEUE);
        }
        // Error handling
        if (!queues[i]) {
            perror("Queue creation failed\n");
            // Free the created queues
            for (int j = 0; j < i; j++) {
                free(queues[j]);
            }
            free(queues);
            return -1;
        }

        // Insert tasks into Queue
        for (size_t j = 0; j < queues[i]->capacity; j++) {

            // Check if we have created the wrong number of tasks
            if (tasks_created > NUM_TASKS) {
                printf("Error: Somehow there are more tasks than intended!\n");
            }

            // Create Task and enqueue
            Task t = {0};
            t.block_size = tasks_created;
            queue_add(queues[i], t);

            // Increment to keep track of tasks created thus far
            tasks_created++;
        }
    }
    printf("%s %zu\n", "Tasks created:", tasks_created);

    // Allocate threads
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
    if (!threads) {
        perror("Error: threads allocation failed");
        return -1;
    }

    // An array containing the arguments for each thread
    ThreadArgs** args_arr = (ThreadArgs**)malloc(sizeof(ThreadArgs*) * NUM_THREADS);
    if (!args_arr) {
        perror("Error: Allocation of args_arr failed\n");
        return -1;
    }

    // Create the arguments for each thread and create the threads
    for (size_t i = 0; i < NUM_THREADS; i++) {

        args_arr[i] = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        if (!args_arr[i]) {
            perror("Error: Args allocation failed");
            return -1;
        }
        // Set member variables
        args_arr[i]->q = queues[i];
        args_arr[i]->FINAL_VALUE = FINAL_VALUE;
        args_arr[i]->arr = arr;
        args_arr[i]->batch_size = BATCH_SIZE;

        // Create thread
        if (pthread_create(&threads[i], NULL, worker_thread_queues_batch, args_arr[i]) != 0) {
            perror("Error: Creating thread failed");
            for (size_t j = 0; j < i; j++) {
                pthread_cancel(threads[j]);
            }
        }

    }

    // Wait for threads to finish
    for (size_t i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("Something went wrong with ptrhead_join\n");
        }
    }

    // Check each value is correct
    for (size_t i = 0; i < NUM_TASKS; i ++) {
        if (arr[i] < FINAL_VALUE) {
            printf("Wrong array value at index %zu\n", i);
            break;
        }
    }

    // Clean up heap
    free(arr);
    free(threads);
    // Free each argument
    for (size_t i = 0; i < NUM_THREADS; i++) {
        free(args_arr[i]);
    }
    free(args_arr);

    // End timer
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate elapsed time in seconds
    elapsed_time = (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%-35s %f\n", "Elapsed time:", elapsed_time);

    return elapsed_time;
}

int main() {

    printf("%s\n", "Starting multi_thread_example.c");

    // Benchmark parameters
    const size_t NUM_RUNS = 1;
    const size_t NUM_TASKS = 1000;
    const size_t NUM_THREADS = 16;
    const size_t FINAL_VALUE = 10000;
    const size_t BATCH_SIZE = 128;

    // Testing with a single Queue
    double total_time = 0;
    for (size_t i = 0; i < NUM_RUNS; i++) {
        total_time += test_single_queue(NUM_TASKS, NUM_THREADS, FINAL_VALUE);
    }

    printf("%-35s %f\n", "test_single_queue average time: ", total_time / NUM_RUNS);

    // Testing with an unique Queue per thread
    total_time = 0;
    for (size_t i = 0; i < NUM_RUNS; i++) {
        total_time += test_local_queues(NUM_TASKS, NUM_THREADS, FINAL_VALUE, BATCH_SIZE);
    }

    printf("%-35s %f\n", "test_local_queues average time: ", total_time / NUM_RUNS);

    printf("%s\n", "Finishing multi_thread_example.c");

    return 0;
}
