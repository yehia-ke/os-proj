#ifndef FCFS_H
#define FCFS_H

#include <stdlib.h>
#include <string.h>
#include "../pcb/pcb.h"
#include "../queue/queue.h"
#include "../interpreter/interpreter.h"
#include "../interpreter/mutex.h"


void initialize_fcfs();
void run_fcfs();
void fcfs_add_process(PCB* process);
void fcfs_wait(char mutex_name[]);
void fcfs_signal(char mutex_name[]);
Queue* fcfs_get_process_queue();
Queue* fcfs_get_ready_queue();
Queue* fcfs_get_block_queue();
Queue* fcfs_get_run_queue();

#endif
