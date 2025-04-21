#include "rr.h"
#include <stdio.h>
#include <stdlib.h>

// Round Robin Scheduler using generic queue
void roundRobinScheduler(Queue* readyQueue, int timeQuantum, int* clockCycle) {
    while (!queue_is_empty(readyQueue)) {
        PCB* process = (PCB*)queue_dequeue(readyQueue);

        set_state(process, "Running");

        // Execute up to `timeQuantum` instructions or until the process finishes
        for (int i = 0; i < timeQuantum; i++) {
            char* instr = get_instruction(process);
            if (!instr) break;

            printf("[Cycle %d] PID %d (PC: %d, Priority: %d) executes: %s\n",
                   *clockCycle, process->pid, process->pc, process->priority, instr);
            (*clockCycle)++;
            process->pc++;
        }

        // Check if there are more instructions to execute
        char* next_instr = get_instruction(process);
        if (next_instr) {
            set_state(process, "Ready");
            printf("Time slice over — requeuing Process %d\n", process->pid);
            queue_enqueue(readyQueue, process);
        } else {
            set_state(process, "Terminated");
            printf("Process %d has finished execution.\n", process->pid);
            free_process(process);
        }
    }
}
