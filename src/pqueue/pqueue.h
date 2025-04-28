#ifndef PQUEUE_H
#define PQUEUE_H

#include "../pcb/pcb.h"

typedef struct PQNode {
    PCB         *pcb;
    struct PQNode *next;
} PQNode;

typedef struct PQueue {
    PQNode *head;
} PQueue;

PQueue* pqueue_create(void);

void pqueue_destroy(PQueue *q);

int pqueue_is_empty(PQueue *q);

void pqueue_enqueue(PQueue *q, PCB *pcb);

PCB* pqueue_dequeue(PQueue *q);

#endif