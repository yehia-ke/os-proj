#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Queue Queue;

Queue* queue_create(void);
bool queue_enqueue(Queue* queue, void* data);
void* queue_dequeue(Queue* queue);
bool queue_is_empty(const Queue* queue);
size_t queue_size(const Queue* queue);
void queue_destroy(Queue* queue);

#endif