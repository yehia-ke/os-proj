#include <stdlib.h>
#include "pqueue.h"

PQueue* pqueue_create(void) {
    PQueue *q = malloc(sizeof(*q));
    if (!q) return NULL;
    q->head = NULL;
    return q;
}

void pqueue_destroy(PQueue *q) {
    if (!q) return;
    PQNode *cur = q->head;
    while (cur) {
        PQNode *next = cur->next;
        free(cur);
        cur = next;
    }
    free(q);
}

int pqueue_is_empty(PQueue *q) {
    return (q->head == NULL);
}

void pqueue_enqueue(PQueue *q, PCB *pcb) {
    if (!q || !pcb) return;
    PQNode *node = malloc(sizeof(*node));
    if (!node) return;
    node->pcb = pcb;
    node->next = NULL;

    /* insert into sorted list (descending priority) */
    if (!q->head || pcb->priority > q->head->pcb->priority) {
        /* new head */
        node->next = q->head;
        q->head = node;
    } else {
        PQNode *cur = q->head;
        while (cur->next && cur->next->pcb->priority >= pcb->priority) {
            cur = cur->next;
        }
        node->next = cur->next;
        cur->next = node;
    }
}

PCB* pqueue_dequeue(PQueue *q) {
    if (!q || !q->head) return NULL;
    PQNode *node = q->head;
    PCB *pcb = node->pcb;
    q->head = node->next;
    free(node);
    return pcb;
}
