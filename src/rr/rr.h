#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H

#include "pcb.h"
#include "queue.h"

// Scheduler function
void roundRobinScheduler(Queue* readyQueue, int timeQuantum, int* clockCycle);

#endif
