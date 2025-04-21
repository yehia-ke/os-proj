#ifndef MLFQ_H
#define MLFQ_H

#include <stdlib.h>
#include "../pcb/pcb.h"
#include "../queue/queue.h"
#include "../interpreter/interpreter.h"

void initialize_mlfq();
void run_mlfq();
void mlfq_add_process(PCB* process);

#endif
