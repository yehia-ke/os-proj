#ifndef ROUND_ROBIN_SCHEDULER_H
#define ROUND_ROBIN_SCHEDULER_H

#include <string.h>

#include "../pcb/pcb.h"
#include "../queue/queue.h"
#include "../interpreter/interpreter.h"
#include "../interpreter/mutex.h"

void initialize_rr();
void run_rr();
void rr_add_process(PCB* process);
void rr_wait(char mutex_name[]);
void rr_signal(char mutex_name[]);
Queue* rr_get_process_queue();

#endif
