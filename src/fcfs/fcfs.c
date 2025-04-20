#include "fcfs.h"
#include <stdio.h>

PCB* running_process = NULL;  // Pointer to the currently running process
Queue* ready_queue = NULL;    // Queue for ready processes

void initialize_fcfs() {
    if (ready_queue == NULL) {
        ready_queue = queue_create();  // Initialize the ready queue
    }
}

void fcfs_add_process(PCB* process) {
    if (ready_queue == NULL) {
        printf("Error: Ready queue is not initialized\n");
        return;
    }
    queue_enqueue(ready_queue, process);
}

void run_fcfs() {
    if (running_process != NULL) {
        char* instruction = get_instruction(running_process);
        if (instruction == NULL) {
            printf("Process %d Finished\n", running_process->pid);
            set_state(running_process, "Terminated");
            free_process(running_process);
            running_process = NULL;
            return;
        } else {
            printf("Process %d running: %s\n", running_process->pid, instruction);
            running_process->pc++;
            execute_instruction(instruction, running_process);
        }
    }

    if (running_process == NULL && !queue_is_empty(ready_queue)) {
        running_process = (PCB*)queue_dequeue(ready_queue);
        set_state(running_process, "Running");
        printf("Process %d is starting\n", running_process->pid);

        char* instruction = get_instruction(running_process);
        if (instruction != NULL) {
            printf("Process %d running: %s\n", running_process->pid, instruction);
            running_process->pc++;
            execute_instruction(instruction, running_process);
        } else {
            printf("Process %d Finished\n", running_process->pid);
            set_state(running_process, "Terminated");
            free_process(running_process);
            running_process = NULL;
        }
    }
}