#include "fcfs_scheduler.h"
#include <stdio.h>

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

void run_fcfs() {

    // Check if there are any processes in the queue
    // If not, return
    // else

    // Dequeue a process from ready queue
    // Update the process state to "Running"
    // Execute the process until it finishes
    // Update num_processes
    // Free the process from memory
    // Recall the function to check for more processes


    /*PCB* process = processes[i];  //We grab the current process from the array
    printf("Running Process %d\n", process->pid);  //Show which process is being run right now
    set_state(process, "Running");  //Update the process state to "Running" in both: Memory and the PCB struct
    char* instruction;  //Declare a variable to hold each instruction
    while ((instruction = get_instruction(process)) != NULL) {  //Get the next instruction based on the process’s PC(If there's an instruction, run the loop, If there's no more, exit the loop)
        printf("Executing: %s\n", instruction);  //Print the instruction so you can see what’s running
        process->pc++;  //Move the PC to the next instruction so next time it gets the following one.
    }
    printf("Process %d Finished\n\n", process->pid);  //Print that the process has finished
    free_process(process);  //Free the memory of that process (both from memory and from the heap)*/
}
