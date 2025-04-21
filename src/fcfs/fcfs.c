#include "fcfs.h"
#include <stdio.h>

PCB* running_process = NULL;  // Pointer to the currently running process
Queue* ready_queue = NULL;    // Queue for ready processes
Queue* waiting_queue = NULL; // Queue for waiting processes

void initialize_fcfs() {
    ready_queue = queue_create(); // Initialize the ready queue
    waiting_queue = queue_create(); // Initialize the waiting queue
}

void fcfs_add_process(PCB* process) {
    if (ready_queue == NULL) {
        printf("Error: Ready queue is not initialized\n");
        return;
    }
    queue_enqueue(ready_queue, process);
}

void fcfs_wait(char mutex_name[]) {
    // Add the process to the waiting queue
    int mutex_index;

    if (strcmp(mutex_name, "userInput") == 0) {
        mutex_index = 0;
    } else if (strcmp(mutex_name, "userOutput") == 0) {
        mutex_index = 1;
    } else if (strcmp(mutex_name, "file") == 0) {
        mutex_index = 2;
    } else {
        printf("Error: Unknown mutex name %s\n", mutex_name);
        return;
    }

    set_state(running_process, "Waiting"); // Set the state to Waiting
    queue_enqueue(waiting_queue, running_process); // Enqueue the process in the waiting queue
    running_process = NULL; // Clear the running process
}

void fcfs_signal(char mutex_name[]) {
    // Signal the mutex and wake up the waiting process
    int mutex_index;

    if (strcmp(mutex_name, "userInput") == 0) {
        mutex_index = 0;
    } else if (strcmp(mutex_name, "userOutput") == 0) {
        mutex_index = 1;
    } else if (strcmp(mutex_name, "file") == 0) {
        mutex_index = 2;
    } else {
        printf("Error: Unknown mutex name %s\n", mutex_name);
        return;
    }

    // Dequeue all processes waiting on this mutex and enqueue them in the ready queue
    while (!queue_is_empty(waiting_queue)) {
        PCB* waiting_process = (PCB*)queue_dequeue(waiting_queue); // Dequeue the first waiting process
        queue_enqueue(ready_queue, waiting_process); // Enqueue it in the ready queue
        set_state(waiting_process, "Ready"); // Set its state to Ready
    }
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