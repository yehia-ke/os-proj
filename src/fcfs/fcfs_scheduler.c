#include "fcfs_scheduler.h"
#include <stdio.h>

PCB* running_process = NULL;  //Pointer to the currently running process
Queue* ready_queue = NULL;  //Queue for ready processes

void initialize_fcfs() {
    ready_queue = queue_create();  // Initialize the ready queue
}

void fcfs_add_process(PCB* process) {  //Function to add a process to the ready queue
    queue_enqueue(ready_queue, process);
}

char* run_fcfs() {

    // Check if the running process is not NULL
        // if true, get the instruction from the running process
            // if returns NULL, Free the process from memory. 
        // Else,
            // Return the instruction
    // else
        // Check if there aren't any processes in the queue using queue_is_empty
            // If ture, return NULL
        // else
            // Dequeue a process from ready queue
            // Update the process state to "Running"
            // Set running_process to the dequeued process
            // Get instruction from the process
            // Return the instruction
}
