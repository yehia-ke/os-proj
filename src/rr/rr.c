#include "rr.h"
#include <stdio.h>

PCB* running_process = NULL;   // Pointer to the currently running process
Queue* ready_queue = NULL;     // Queue for ready processes
Queue* waiting_queue[3];   // Queue for waiting processes
int time_quantum = 2;          // Default time quantum
int time_slice = 0;            // Time slice counter

void initialize_rr() {
    ready_queue = queue_create();  // Initialize the ready queue
    for (int i = 0; i < 3; i++) {
        waiting_queue[i] = queue_create(); // Initialize each waiting queue
    }
}

void rr_add_process(PCB* process) {
    set_state(process, "Ready");
    queue_enqueue(ready_queue, process);
}

void run_rr() {
    // If there's a running process already
    if (running_process != NULL) {
        char* instruction = get_instruction(running_process);

        if (instruction == NULL) {
            printf("Process %d Finished\n", running_process->pid);
            set_state(running_process, "Terminated");
            free_process(running_process);
            running_process = NULL;
            time_slice = 0;
            return;
        }

        printf("Process %d running: %s\n", running_process->pid, instruction);
        running_process->pc++;
        time_slice++;

        execute_instruction(instruction, running_process);  // Execute the instruction

        if (time_slice >= time_quantum) {
            // Time quantum exhausted, requeue the process
            printf("Quantum expired for process %d\n", running_process->pid);
            set_state(running_process, "Ready");
            queue_enqueue(ready_queue, running_process);
            running_process = NULL;
            time_slice = 0;
        }
    }

    // If no process is currently running
    if (queue_is_empty(ready_queue)) {
        printf("No processes in the ready queue\n");
        return;
    }

    running_process = (PCB*)queue_dequeue(ready_queue);
    set_state(running_process, "Running");
    printf("Process %d is starting\n", running_process->pid);

    char* instruction = get_instruction(running_process);
    if (instruction != NULL) {
        printf("Process %d running: %s\n", running_process->pid, instruction);
        running_process->pc++;  // Move to next instruction for future calls
        time_slice++;
        execute_instruction(instruction, running_process);  // Execute the instruction

        if(time_slice >= time_quantum) {
            // Time slice expired, move to the next queue
            set_state(running_process, "Ready");
            queue_enqueue(ready_queue, running_process);
            printf("Quantum expired for process %d\n", running_process->pid);
            running_process = NULL;
            time_slice = 0;  // Reset time slice
        }
    }
    else {
        printf("No instruction to run... Terminating\n");
        set_state(running_process, "Terminated"); // Set state to Terminated
        free_process(running_process);
        running_process = NULL;
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

    set_state(running_process, "Waiting"); // Set the state to Waiting
    running_process->pc--; // Decrement the program counter to re-execute the instruction
    queue_enqueue(waiting_queue[mutex_index], running_process); // Enqueue the process in the waiting queue
    running_process = NULL; // Clear the running process
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
    while (!queue_is_empty(waiting_queue[mutex_index])) {
        PCB* waiting_process = (PCB*)queue_dequeue(waiting_queue[mutex_index]); // Dequeue the first waiting process
        queue_enqueue(ready_queue, waiting_process); // Enqueue it in its respective priority queue
        set_state(waiting_process, "Ready"); // Set its state to Ready
    }
}
