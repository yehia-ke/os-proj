#ifndef MLFQ_SCHEDULER_H
#define MLFQ_SCHEDULER_H

#include <stdlib.h>
#include "../pcb/pcb.h"
#include "../queue/queue.h"

void initialize_mlfq();
char* run_mlfq();
void mlfq_add_process(PCB* process);

#endif
