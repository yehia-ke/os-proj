#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string.h>
#include <stdlib.h>
#include "../pcb/pcb.h"
#include "../fcfs/fcfs.h"
#include "../mlfq/mlfq.h"
#include "../rr/rr.h"
#include "../queue/queue.h"

void set_scheduler(char* scheduler_name);
char* run_scheduler();
void add_process_to_scheduler(PCB* process);
void scheduler_wait(char mutex_name[]);
void scheduler_signal(char mutex_name[]);
Queue* get_process_queue();
Queue* get_ready_queue();
Queue* get_block_queue();
Queue* get_run_queue();


#endif