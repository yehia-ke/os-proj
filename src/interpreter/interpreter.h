#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "../pcb/pcb.h"
#include "mutex.h"
#include "../memory/memory.h"
// Function to execute a single instruction for a given process
void execute_instruction(char *instruction, PCB *process);

#endif // INTERPRETER_H