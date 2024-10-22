#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include "stdbool.h"
#include "task.h"

typedef struct {

    // The elements in the queue
    Task* elements;

    // The current size of the queue (in elements)
    size_t size;

    // The capacity of the queue (in elements)
    size_t capacity;


} Queue;

Queue* queue_create(size_t capacity);

int queue_add(Queue* q, Task* t);

Task* queue_get(Queue* q);

Task* queue_peek(Queue* q);

bool queue_is_empty(Queue* q);

int queue_free(Queue* q);

#endif // QUEUE_H
