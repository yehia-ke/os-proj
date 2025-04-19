#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdlib.h>
#include "../pcb/pcb.h"
#include "../fcfs/fcfs_scheduler.h"
#include "../mlfq/mlfq.h"
// #include "../rr/rr.h"

void set_scheduler(char* scheduler_name);
char* run_scheduler();
void add_process_to_scheduler(PCB* process);

#endif