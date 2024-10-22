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
        printf("%s\n", "The Queue is full");
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
        return empty_task;
    }

    // Retrieve the Task from the array
    Task t;
    memcpy(&t, &q->elements[q->front], sizeof(Task));

    // Increment the rear of the Queue
    q->front = (q->front + 1) % q->capacity;
    q->size--;

    return t;
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
