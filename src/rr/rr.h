#ifndef ROUND_ROBIN_SCHEDULER_H
#define ROUND_ROBIN_SCHEDULER_H

#include "../pcb/pcb.h"
#include "../queue/queue.h"

void initialize_round_robin(int quantum);
char* run_round_robin();
void round_robin_add_process(PCB* process);

#endif
