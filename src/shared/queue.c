#include "queue.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

Queue* queue_create(size_t capacity) {

    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (!q) {
        errno = ENOMEM;
        perror("Error: Allocation of Queue failed");
        return NULL;
    }

    Task* elements = (Task*)malloc(sizeof(Task) * capacity);
    if (!elements) {
        errno = ENOMEM;
        perror("Error: Allocation of elements in Queue failed");
        free(q);
        return NULL;
    }

    // Set member variables
    q->elements = elements;
    q->size = 0;
    q->capacity = capacity;
    q->front = 0;
    q->rear = 0;

    return q;
}

int queue_add(Queue* q, Task t) {

    if (!q) {
        errno = EINVAL;
        perror("Error: Queue argument missing");
        return -1;
    }

    if (q->size >= q->capacity) {
        // The Queue is full
        printf("%s\n", "Warning: The Queue is full");
        return -1;
    }

    // Copy the Task object into the Queue
    memcpy(&q->elements[q->rear], &t, sizeof(Task));

    // Increment the rear of the Queue
    q->rear = (q->rear + 1) % q->capacity;
    q->size++;

    return 0;
}

Task queue_get(Queue* q) {

    if (!q) {
        errno = EINVAL;
        perror("Error: Queue argument missing");
        Task empty_task = {0};
        return empty_task;
    }

    if (queue_is_empty(q)) {
        Task empty_task = {0};
        printf("The Queue is empty!\n");
        return empty_task;
    }

    // Retrieve the Task from the array
    Task t;
    memcpy(&t, &q->elements[q->front], sizeof(Task));

    // Increment the front of the Queue
    q->front = (q->front + 1) % q->capacity;
    q->size--;

    return t;
}

Task* queue_get_batch(Queue* q, size_t batch_size) {

    if (!q || batch_size == 0) {
        errno = EINVAL;
        perror("Error: No Queue or batch_size is 0\n");
        return NULL;
    }

    // Check if we should resize batch based on Queue size
    if (batch_size > q->size) {
        batch_size = q->size;
    }

    // Allocate array to store batch
    Task* batch = (Task*)malloc(sizeof(Task) * batch_size);
    if (!batch) {
        perror("Error: Failed to allocate batch array for queue\n");
        return NULL;
    }

    // Insert the tasks into batch array
    for (size_t i = 0; i < batch_size; i++) {

        // Copy the tasks into the batch array
        memcpy(&batch[i], &q->elements[q->front], sizeof(Task));

        // Update Queue front pointer
        q->front = (q->front + 1) % q->capacity;
        q->size--;
    }

    return batch;
}

Task queue_peek(Queue* q) {

    if (!q) {
        errno = EINVAL;
        perror("Error: Queue argument missing");
        Task empty_task = {0};
        return empty_task;
    }

    if (queue_is_empty(q)) {
        // The Queue is empty
        Task empty_task = {0};
        return empty_task;
    }

    // Retrieve the Task from the array
    Task t;
    memcpy(&t, &q->elements[q->front], sizeof(Task));

    return t;
}

bool queue_is_empty(Queue* q) {

    if (!q) {
        errno = EINVAL;
        perror("Error: Queue argument missing");
        return true;
    }

    return (q->size == 0);
}

int queue_free(Queue* q) {

    if (!q) {
        errno = EINVAL;
        perror("Error: Queue argument missing");
        return -1;
    }

    free(q->elements);
    free(q);
    return 0;
}
