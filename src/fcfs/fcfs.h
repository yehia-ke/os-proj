#ifndef FCFS_H
#define FCFS_H
#include <stdlib.h>
#include "../pcb/pcb.h"
#include "../queue/queue.h"
#include "../interpreter/interpreter.h"

void initialize_fcfs();
void run_fcfs();
void fcfs_add_process(PCB* process);

#endif
