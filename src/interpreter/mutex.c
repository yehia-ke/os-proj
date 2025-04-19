#include "mutex.h"
#include <string.h>

// Define the mutex variables
Mutex userInput = { "" };   // Initially no owner
Mutex userOutput = { "" };  // Initially no owner
Mutex file = { "" };        // Initially no owner

int is_owner(Mutex* mutex, PCB* pcb) {
    // Check if the mutex has no owner
    if (strlen(mutex->owner) == 0) {
        // Assign the PCB as the owner
        strcpy(mutex->owner, getPID(pcb)); // Assuming PCB has a 'name' field
        return 1; // Successfully assigned ownership
    }

    // Check if the given PCB is already the owner
    if (strcmp(mutex->owner, getPID(pcb)) == 0) {
        return 1; // PCB is already the owner
    }

    // Otherwise, the PCB is not the owner
    return 0;
}