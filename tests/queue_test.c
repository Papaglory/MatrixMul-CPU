#include "../src/shared/task.h"
#include "../src/shared/queue.h"
#include <stdio.h>

void print_stats(Queue* q) {

    printf("%s\n", "Queue stats");
    printf("%s %p\n", "elements", q->elements);
    printf("%s %zu\n", "size", q->size);
    printf("%s %zu\n", "capacity", q->capacity);
    printf("%s %zu\n", "front", q->front);
    printf("%s %zu\n", "rear", q->rear);
    printf("\n");
}

int main() {

    printf("%s\n\n", "--------STARTING queue_test.c--------");

    printf("%s\n", "Creating the queue");
    Queue* q = queue_create(3);
    Task t0 = {0};
    t0.block_size = 0;
    Task t1 = {0};
    t1.block_size = 1;
    Task t2 = {0};
    t2.block_size = 2;
    Task t3 = {0};
    t3.block_size = 3;

    print_stats(q);

    queue_add(q, t0);
    print_stats(q);

    queue_add(q, t1);
    print_stats(q);

    queue_add(q, t2);
    print_stats(q);

    Task ret = queue_get(q);
    printf("%s %zu\n", "Task id:", ret.block_size);
    print_stats(q);

    queue_add(q, t3);
    print_stats(q);

    queue_add(q, t3);
    print_stats(q);

    Task re = queue_get(q);
    printf("%s %zu\n", "Task id:", ret.block_size);
    print_stats(q);

    Task e = queue_get(q);
    printf("%s %zu\n", "Task id:", ret.block_size);
    print_stats(q);

    Task r = queue_get(q);
    printf("%s %zu\n", "Task id:", ret.block_size);
    print_stats(q);

    Task l = queue_get(q);
    printf("%s %zu\n", "Task id:", ret.block_size);
    print_stats(q);

    return 0;
}
