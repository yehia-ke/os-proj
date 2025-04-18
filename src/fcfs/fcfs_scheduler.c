#include "fcfs_scheduler.h"
#include <stdio.h>

PCB* running_process = NULL;  //Pointer to the currently running process
PCB* processes[20];  //Turn this into a Queue
int num_processes = 0;  //Number of processes in the array

void add_process(PCB* process) {  //Function to add a process to the array
    if (num_processes < 20) {  //Check if there is space in the array
        processes[num_processes] = process;  //Add the process to the array
        num_processes++;  //Increment the number of processes
    } else {
        printf("Error: Process limit reached.\n");  //If there is no space, print an error message
    }
}

char* run_fcfs() {

    // Check if the running process is not NULL
    // if true, get the instruction from the running process
    // If returns NULL, Update num_processes & Free the process from memory. Else,
    // Return the instruction
    // else

    // Check if there are any processes in the queue
    // If not, return NULL
    // else

    // Dequeue a process from ready queue
    // Update the process state to "Running"
    // Set the running process to the dequeued process
    // Get instruction from the process
    // Return the instruction
}
