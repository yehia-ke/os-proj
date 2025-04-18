#ifndef FCFS_SCHEDULER_H
#define FCFS_SCHEDULER_H
#include <stdlib.h>
#include "../pcb/pcb.h"
#include "../queue/queue.h"

void initialize_fcfs();
char* run_fcfs();
void fcfs_add_process(PCB* process);

#endif
