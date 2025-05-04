#include "mutex.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Define the mutex variables
Mutex userInput = { "" };   // Initially no owner
Mutex userOutput = { "" };  // Initially no owner
Mutex file = { "" };        // Initially no owner

int is_owner(Mutex* mutex, PCB* pcb) {
    char tmp[50];
    // Check if the mutex has no owner
    if (strlen(mutex->owner) == 0) {
        // Assign the PCB as the owner
        sprintf(tmp, "%d", pcb->pid);
        strcpy(mutex->owner, tmp);
        return 1; // Successfully assigned ownership
    }

    // Check if the given PCB is already the owner
    sprintf(tmp, "%d", pcb->pid); // Format PCB pid as a string
    if (strcmp(mutex->owner, tmp) == 0) {
        return 1; // PCB is already the owner
    }

    // Otherwise, the PCB is not the owner
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
            return "Invalid mutex index"; // Return error message for invalid index
    }
}