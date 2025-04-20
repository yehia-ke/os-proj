#ifndef MUTEX_H
#define MUTEX_H

#include "../pcb/pcb.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char owner[50];
} Mutex;

extern Mutex userInput;
extern Mutex userOutput;
extern Mutex file;

int is_owner(Mutex* mutex, PCB* pcb);

#endif 