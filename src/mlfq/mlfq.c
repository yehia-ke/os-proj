#include "mlfq.h"
#include <stdio.h>

PCB* running_process = NULL;  // Pointer to the currently running process
Queue* ready_queue[4];  // Array of queues for each priority level
int time_quantum[4] = {1, 2, 4, 8};  // Time quantum for each priority level

int time_slice = 0;  // Time slice counter

void initialize_mlfq() {
    for (int i = 0; i < 4; i++) {
        ready_queue[i] = queue_create();  // Initialize each priority queue
    }
}

void mlfq_add_process(PCB* process) {
    // Add the process to the highest priority queue (0)
    queue_enqueue(ready_queue[0], process);
}

char* run_mlfq() {
    if (running_process != NULL) {
        // Get the next instruction from the running process
        char* instruction = get_instruction(running_process);

        // If no more instructions: mark it done and free it
        if (instruction == NULL) {
            printf("Process %d Finished\n", running_process->pid);
            free_process(running_process);
            running_process = NULL;
            return NULL;
        } else {
            // If there's still an instruction, return it and increment PC
            printf("Process %d running: %s\n", running_process->pid, instruction);
            running_process->pc++;
            time_slice++;

            if(time_slice >= time_quantum[running_process->priority]) {
                // Time slice expired, move to the next queue
                set_state(running_process, "Ready");
                
                queue_enqueue(ready_queue[running_process->priority - 1], running_process);
                running_process = NULL;
                time_slice = 0;  // Reset time slice
            }
            
            return instruction;
        }
    }

    // If no process is running, check if the highest priority queue is empty
    if (queue_is_empty(ready_queue[0])) {
        return NULL;  
    }

    // If the highest priority queue isn't empty, start the next process
    running_process = (PCB*)queue_dequeue(ready_queue[0]);  // Get the next process
    set_state(running_process, "Running");                   // Set its state to running
    printf("Process %d is starting\n", running_process->pid);

    // Get its first instruction
    char* instruction = get_instruction(running_process);
    if (instruction != NULL) {
        printf("Process %d running: %s\n", running_process->pid, instruction);
        running_process->pc++;  // Move to next instruction for future calls
    }

    return instruction;  // Return the instruction (or NULL if none)
}