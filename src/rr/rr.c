#include "rr.h"
#include <stdio.h>

PCB* rr_running_process = NULL;   // Pointer to the currently running process
Queue* rr_ready_queue = NULL;     // Queue for ready processes
Queue* rr_waiting_queue[3];   // Queue for waiting processes
int rr_time_quantum = 2;          // Default time quantum
int rr_time_slice = 0;            // Time slice counter

void initialize_rr() {
    rr_ready_queue = queue_create();  // Initialize the ready queue
    for (int i = 0; i < 3; i++) {
        rr_waiting_queue[i] = queue_create(); // Initialize each waiting queue
    }
}

void rr_add_process(PCB* process) {
    set_state(process, "Ready");
    queue_enqueue(rr_ready_queue, process);
}

void run_rr() {
    // If there's a running process already
    if (rr_running_process != NULL) {
        char* instruction = get_instruction(rr_running_process);

        if (instruction == NULL) {
            printf("Process %d Finished\n", rr_running_process->pid);
            set_state(rr_running_process, "Terminated");
            free_process(rr_running_process);
            rr_running_process = NULL;
            rr_time_slice = 0;
            return;
        }

        printf("Process %d running: %s\n", rr_running_process->pid, instruction);
        increment_pc(rr_running_process);
        rr_time_slice++;

        execute_instruction(instruction, rr_running_process);  // Execute the instruction

        if (rr_time_slice >= rr_time_quantum) {
            // Time quantum exhausted, requeue the process
            printf("Quantum expired for process %d\n", rr_running_process->pid);
            set_state(rr_running_process, "Ready");
            queue_enqueue(rr_ready_queue, rr_running_process);
            rr_running_process = NULL;
            rr_time_slice = 0;
        }
    }

    // If no process is currently running
    if (queue_is_empty(rr_ready_queue)) {
        printf("No processes in the ready queue\n");
        return;
    }

    rr_running_process = (PCB*)queue_dequeue(rr_ready_queue);
    set_state(rr_running_process, "Running");
    printf("Process %d is starting\n", rr_running_process->pid);

    char* instruction = get_instruction(rr_running_process);
    if (instruction != NULL) {
        printf("Process %d running: %s\n", rr_running_process->pid, instruction);
        increment_pc(rr_running_process);
        rr_time_slice++;
        execute_instruction(instruction, rr_running_process);  // Execute the instruction

        if(rr_time_slice >= rr_time_quantum) {
            // Time slice expired, move to the next queue
            set_state(rr_running_process, "Ready");
            queue_enqueue(rr_ready_queue, rr_running_process);
            printf("Quantum expired for process %d\n", rr_running_process->pid);
            rr_running_process = NULL;
            rr_time_slice = 0;  // Reset time slice
        }
    }
    else {
        printf("No instruction to run... Terminating\n");
        set_state(rr_running_process, "Terminated"); // Set state to Terminated
        free_process(rr_running_process);
        rr_running_process = NULL;
        return;
    }
}

void rr_wait(char mutex_name[]) {
    // Add the process to the waiting queue

    int mutex_index;

    if (strcmp(mutex_name, "userInput") == 0) {
        mutex_index = 0;
    } else if (strcmp(mutex_name, "userOutput") == 0) {
        mutex_index = 1;
    } else if (strcmp(mutex_name, "file") == 0) {
        mutex_index = 2;
    }
    else
    {
        printf("Error: Unknown mutex name %s\n", mutex_name);
        return;
    }

    set_state(rr_running_process, "Waiting"); // Set the state to Waiting
    decrement_pc(rr_running_process); // Decrement the program counter
    queue_enqueue(rr_waiting_queue[mutex_index], rr_running_process); // Enqueue the process in the waiting queue
    rr_running_process = NULL; // Clear the running process
}

void rr_signal(char mutex_name[]) {
    // Signal the mutex and wake up the waiting process
    int mutex_index;

    if (strcmp(mutex_name, "userInput") == 0) {
        mutex_index = 0;
    } else if (strcmp(mutex_name, "userOutput") == 0) {
        mutex_index = 1;
    } else if (strcmp(mutex_name, "file") == 0) {
        mutex_index = 2;
    }
    else
    {
        printf("Error: Unknown mutex name %s\n", mutex_name);
        return;
    }

    // Dequeue all processes waiting on this mutex and enqueue them in their respective ready queues
    while (!queue_is_empty(rr_waiting_queue[mutex_index])) {
        PCB* waiting_process = (PCB*)queue_dequeue(rr_waiting_queue[mutex_index]); // Dequeue the first waiting process
        queue_enqueue(rr_ready_queue, waiting_process); // Enqueue it in its respective priority queue
        set_state(waiting_process, "Ready"); // Set its state to Ready
    }
}
