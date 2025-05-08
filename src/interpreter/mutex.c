#include "mutex.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Mutex userInput = { "" };  
Mutex userOutput = { "" }; 
Mutex file = { "" };

int is_owner(Mutex* mutex, PCB* pcb) {
    char tmp[50];
    if (strlen(mutex->owner) == 0) {
        sprintf(tmp, "%d", pcb->pid);
        strcpy(mutex->owner, tmp);
        return 1;
    }

    sprintf(tmp, "%d", pcb->pid);
    if (strcmp(mutex->owner, tmp) == 0) {
        return 1; 
    }

    return 0;
}

const char* getMutex(int i) {
    switch (i) {
        case 0:
            return userInput.owner;
        case 1:
            return userOutput.owner;
        case 2:
            return file.owner;
        default:
            return "Invalid mutex index";
    }
}