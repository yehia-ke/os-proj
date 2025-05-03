#ifndef MLFQ_H
#define MLFQ_H

#include <stdlib.h>
#include <string.h>
#include "../pcb/pcb.h"
#include "../queue/queue.h"
#include "../interpreter/interpreter.h"
#include "../interpreter/mutex.h"
#include "../pqueue/pqueue.h"

void initialize_mlfq();
void run_mlfq();
void mlfq_add_process(PCB* process);
void mlfq_wait(char mutex_name[]);
void mlfq_signal(char mutex_name[]);
Queue* mlfq_get_process_queue();
Queue* mlfq_get_ready_queue();
Queue* mlfq_get_block_queue();
Queue* mlfq_get_run_queue();

#endif
