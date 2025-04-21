#include "queue.h"
#include <stdlib.h>

typedef struct Node {
    void* data;
    struct Node* next;
} Node;

struct Queue {
    Node* front;
    Node* rear;
    size_t size;
};

Queue* queue_create(void) {
    Queue* queue = malloc(sizeof(*queue));
    if (queue) {
        queue->front = NULL;
        queue->rear = NULL;
        queue->size = 0;
    }
    return queue;
}

bool queue_enqueue(Queue* queue, void* data) {
    if (!queue) return false;

    Node* newNode = malloc(sizeof(*newNode));
    if (!newNode) return false;

    newNode->data = data;
    newNode->next = NULL;

    if (queue->rear) {
        queue->rear->next = newNode;
    } else {
        queue->front = newNode;
    }
    queue->rear = newNode;
    queue->size++;
    return true;
}

void* queue_dequeue(Queue* queue) {
    if (queue_is_empty(queue)) return NULL;

    Node* nodeToRemove = queue->front;
    void* data = nodeToRemove->data;
    queue->front = nodeToRemove->next;

    if (!queue->front) queue->rear = NULL;

    queue->size--;
    free(nodeToRemove);
    return data;
}

bool queue_is_empty(const Queue* queue) {
    return !queue || queue->size == 0;
}

size_t queue_size(const Queue* queue) {
    return queue ? queue->size : 0;
}

void queue_destroy(Queue* queue) {
    if (!queue) return;

    Node* current = queue->front;
    while (current) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    free(queue);
}