#include "rr.h"
#include <stdio.h>

PCB* running_process = NULL;   // Pointer to the currently running process
Queue* ready_queue = NULL;     // Queue for ready processes
int time_quantum = 2;          // Default time quantum
int time_slice = 0;            // Time slice counter

void initialize_round_robin(int quantum) {
    ready_queue = queue_create();  // Initialize the ready queue
    time_quantum = quantum;        // Set the user-defined quantum
    time_slice = 0;
    running_process = NULL;
}

void round_robin_add_process(PCB* process) {
    set_state(process, "Ready");
    queue_enqueue(ready_queue, process);
}

char* run_round_robin() {
    // If there's a running process already
    if (running_process != NULL) {
        char* instruction = get_instruction(running_process);

        if (instruction == NULL) {
            printf("Process %d Finished\n", running_process->pid);
            set_state(running_process, "Terminated");
            free_process(running_process);
            running_process = NULL;
            time_slice = 0;
            return NULL;
        }

        printf("Process %d running: %s\n", running_process->pid, instruction);
        running_process->pc++;
        time_slice++;

        if (time_slice >= time_quantum) {
            // Time quantum exhausted, requeue the process
            printf("Quantum expired for process %d\n", running_process->pid);
            set_state(running_process, "Ready");
            queue_enqueue(ready_queue, running_process);
            running_process = NULL;
            time_slice = 0;
        }

        return instruction;
    }

    // If no process is currently running
    if (queue_is_empty(ready_queue)) {
        printf("No processes in the ready queue\n");
        return NULL;
    }

    running_process = (PCB*)queue_dequeue(ready_queue);
    set_state(running_process, "Running");
    printf("Process %d is starting\n", running_process->pid);

    char* instruction = get_instruction(running_process);
    if (instruction != NULL) {
        printf("Process %d running: %s\n", running_process->pid, instruction);
        running_process->pc++;
        time_slice = 1;  // Start counting this quantum
        return instruction;
    } else {
        printf("Process %d Finished\n", running_process->pid);
        set_state(running_process, "Terminated");
        free_process(running_process);
        running_process = NULL;
        time_slice = 0;
        return NULL;
    }
}
