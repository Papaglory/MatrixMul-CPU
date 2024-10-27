#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include "stdbool.h"
#include "task.h"

typedef struct {

    // The elements in the Queue
    Task* elements;

    // The current size of the Queue (in elements)
    size_t size;

    // The capacity of the Queue (in elements)
    size_t capacity;

    // Index to the front of the Queue (next to be served)
    size_t front;

    // Index to the rear of the Queue (used for inserting)
    size_t rear;

} Queue;

/**
 * @brief Create a Queue on the heap with the given input capacity.
 * The capacity represents the capacity of Task objects.
 *
 * @param capacity The number of Task objects the Queue can hold.
 * @return A pointer to the Queue.
*/
Queue* queue_create(size_t capacity);

/**
 * @brief Add Task t to the Queue if there is space.
 *
 * @param q The Queue to add the Task to.
 * @param t The Task to be added. It is passed by value.
 * @return A value of zero for success and -1 for failure.
*/
int queue_add(Queue* q, Task t);

/**
 * @brief Retrieve the Task that is at the front of the Queue.
 * If the Queue is empty, the empty_task is returned
 * (Task empty_task = {0}).
 *
 * @param q The Queue to retrieve the task from.
 * @return The Task passed as value. If an error occured, the empty_task.
*/
Task queue_get(Queue* q);

/**
 * @brief Retrieve a batch of the tasks that start at the front of the Queue.
 * If the Queue is empty, the empty_task is returned
 * (Task empty_task = {0}).
 *
 * @note The caller is responsible for freeing the batch array.
 *
 * @param q The Queue to retrieve the tasks from.
 * @param batch_size The number of tasks to retrieve.
 * @return A pointer to an array on the heap with the tasks. If the number
 * of tasks in the Queue is smaller than the batch size, the remaining tasks
 * will be placed in the returning array. NULL if an error occured.
*/
Task* queue_get_batch(Queue* q, size_t batch_size);

/**
 * @brief Retrieve the Task that is at the front of the Queue
 * without deleting the Task from the Queue.
 * If the Queue is empty, the empty_task is returned
 * (Task empty_task = {0}).
 *
 * @param q The Queue to retrieve the task from.
 * @return The Task passed as value. If an error occured, the empty_task.
*/
Task queue_peek(Queue* q);

/**
 * @brief Determines if the Queue is empty.
 *
 * @param q The Queue under consideration.
 * @return True if it is empty, false if not. True if an error occured.
*/
bool queue_is_empty(Queue* q);

/**
 * @brief Free the Queue from memory.
 *
 * @param q The Queue to free.
 * @return A value of zero for success and -1 if an error occured.
*/
int queue_free(Queue* q);

#endif // QUEUE_H
