#include "fcfs.h"
#include <stdio.h>

PCB* fcfs_running_process = NULL;  // Pointer to the currently running process
Queue* fcfs_ready_queue = NULL;    // Queue for ready processes
Queue* fcfs_waiting_queue[3]; // Queue for waiting processes
Queue* fcfs_process_queue = NULL; // Queue for all processes

void initialize_fcfs() {
    fcfs_ready_queue = queue_create();
    fcfs_process_queue = queue_create(); // Initialize the ready queue
    for (int i = 0; i < 3; i++) {
        fcfs_waiting_queue[i] = queue_create(); // Initialize each waiting queue
    }
}

void fcfs_add_process(PCB* process) {
    if (fcfs_ready_queue == NULL) {
        printf("Error: Ready queue is not initialized\n");
        return;
    }
    queue_enqueue(fcfs_ready_queue, process);
    queue_enqueue(fcfs_process_queue, process); // Enqueue the process in the process queue
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

    set_state(fcfs_running_process, "Waiting"); // Set the state to Waiting
    queue_enqueue(fcfs_waiting_queue[mutex_index], fcfs_running_process); // Enqueue the process in the waiting queue
    fcfs_running_process = NULL; // Clear the running process
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

    char tmp[50];

    if(fcfs_waiting_queue[mutex_index] == NULL || queue_is_empty(fcfs_waiting_queue[mutex_index])) {
        printf("No processes waiting on mutex %s\n", mutex_name);
        strcpy(mutex_name, ""); // Clear the mutex name
        return;
    }


    PCB* waiting_process = (PCB*)queue_dequeue(fcfs_waiting_queue[mutex_index]); // Dequeue the first waiting process
    // Enqueue it in its respective priority queue
    queue_enqueue(fcfs_ready_queue, waiting_process);
    sprintf(tmp, "%d", waiting_process->pid);
    strcpy(mutex_name, tmp); // Copy the process ID to the mutex name
    set_state(waiting_process, "Ready"); // Set its state to Ready
}

void run_fcfs() {
    if (fcfs_running_process != NULL) {
        char* instruction = get_instruction(fcfs_running_process);
        if (instruction == NULL) {
            printf("Process %d Finished\n", fcfs_running_process->pid);
            set_state(fcfs_running_process, "Terminated");
            free_process(fcfs_running_process);
            fcfs_running_process = NULL;
            queue_dequeue(fcfs_process_queue);
            return;
        } else {
            printf("Process %d running: %s\n", fcfs_running_process->pid, instruction);
            increment_pc(fcfs_running_process);
            execute_instruction(instruction, fcfs_running_process);
        }
    }

    if (fcfs_running_process == NULL && !queue_is_empty(fcfs_ready_queue)) {
        fcfs_running_process = (PCB*)queue_dequeue(fcfs_ready_queue);
        set_state(fcfs_running_process, "Running");
        printf("Process %d is starting\n", fcfs_running_process->pid);

        char* instruction = get_instruction(fcfs_running_process);
        if (instruction != NULL) {
            printf("Process %d running: %s\n", fcfs_running_process->pid, instruction);
            increment_pc(fcfs_running_process);
            execute_instruction(instruction, fcfs_running_process);
        } else {
            printf("Process %d Finished\n", fcfs_running_process->pid);
            set_state(fcfs_running_process, "Terminated");
            free_process(fcfs_running_process);
            fcfs_running_process = NULL;
        }
    }
}

Queue* fcfs_get_process_queue() {
    printf("Process Returned");
    return fcfs_process_queue;
}