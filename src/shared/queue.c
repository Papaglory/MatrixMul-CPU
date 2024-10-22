#include "queue.h"
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
    q->capacity = capacity;
    q->size = 0;
    q->elements = elements;

    return q;
}

int queue_add(Queue* q, Task* t);

Task* queue_get(Queue* q);

Task* queue_peek(Queue* q);

bool queue_is_empty(Queue* q);

int queue_free(Queue* q);
