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

    if (running_process != NULL) {

        // Get the next instruction from the running process
        char* instruction = get_instruction(running_process);

        //If no more instructions: mark it done and free it
        if (instruction == NULL) {
            printf("Process %d Finished\n", running_process->pid);
            free_process(running_process);
            running_process = NULL;
            return NULL;
        } else {
            //If there's still an instruction, return it and increment PC
            printf("Process %d running: %s\n", running_process->pid, instruction);
            running_process->pc++;
            return instruction;
        }
    }

    //If no process is running, check if the queue is empty
    if (queue_is_empty(ready_queue)) {
        return NULL;  
    }

    //If the queue isn't empty, start the next process
    running_process = (PCB*)queue_dequeue(ready_queue);  // Get the next process
    set_state(running_process, "Running");                // Set its state to running
    printf("Process %d is starting\n", running_process->pid);

    // Get its first instruction
    char* instruction = get_instruction(running_process);
    if (instruction != NULL) {
        printf("Process %d running: %s\n", running_process->pid, instruction);
        running_process->pc++;  // Move to next instruction for future calls
    }

    return instruction;  // Return the instruction (or NULL if none)
}


