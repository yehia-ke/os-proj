#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "../pcb/pcb.h"
#include "mutex.h"
#include "../memory/memory.h"
void execute_instruction(char *instruction, PCB *process);

#endif