#ifndef PCB_H
#define PCB_H

#include <stdlib.h>
#include "../memory/memory.h"

typedef struct {
    int pid;
    char* state;
    int priority;     
    int pc;
    int mem_lower;
    int mem_upper;
    int arrival_time;
} PCB;

PCB* create_process(int pid, char* program_file, int arrival_time);
void set_state(PCB* pcb, char* new_state);
void set_priority(PCB* pcb, int new_priority);
char* get_instruction(PCB* pcb);
void set_variable(PCB* pcb, char* var_name, char* value);
void free_process(PCB* pcb);
void increment_pc(PCB* pcb);
void decrement_pc(PCB* pcb);
#endif