#ifndef THREAD_ARGS_H
#define THREAD_ARGS_H

#include "queue.h"
#include "matrix.h"

typedef struct {

    Queue* q;
    Matrix* C;

} ThreadArgs;

#endif // THREAD_ARGS_H
