#ifndef PCB_H
#define PCB_H

#include <stdlib.h>

#define STATE_READY "Ready"
#define STATE_RUNNING "Running"
#define STATE_BLOCKED "Blocked"
#define STATE_WAITING "Waiting"

typedef struct {
    int pid;
    char* state;
    int priority;     
    int pc;
    int mem_lower;
    int mem_upper;
} PCB;

PCB* create_process(int pid, char* program_file, int arrival_time);
void set_state(PCB* pcb, char* new_state);
void set_priority(PCB* pcb, int new_priority);
char* get_instruction(PCB* pcb);
void set_variable(PCB* pcb, char* var_name, char* value);
void free_process(PCB* pcb);

#endif