#include "rr.h"
#include <stdio.h>

PCB* rr_running_process = NULL;   // Pointer to the currently running process
Queue* rr_ready_queue = NULL;     // Queue for ready processes
Queue* rr_waiting_queue[3];   // Queue for waiting processes
int rr_time_quantum = 2;          // Default time quantum
int rr_time_slice = 0;

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

        if(!rr_running_process)
        {
            rr_time_slice = 0; // Reset time slice
            return;
        }

        if(rr_running_process->pc == rr_running_process->mem_upper - rr_running_process->mem_lower - 8)
        {
            // Process has finished executing
            printf("Process %d Finished\n", rr_running_process->pid);
            set_state(rr_running_process, "Terminated"); // Set state to Terminated
            free_process(rr_running_process);
            rr_running_process = NULL;
            rr_time_slice = 0; // Reset time slice
            return;
        }

        if (rr_time_slice >= rr_time_quantum) {
            // Time quantum exhausted, requeue the process
            printf("Quantum expired for process %d\n", rr_running_process->pid);
            set_state(rr_running_process, "Ready");
            queue_enqueue(rr_ready_queue, rr_running_process);
            rr_running_process = NULL;
            rr_time_slice = 0;
        }
        return; // Exit the function to avoid running the next process
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

        if(rr_running_process == NULL)
        {
            rr_time_slice = 0; // Reset time slice
            return;
        }

        if(rr_running_process->pc == rr_running_process->mem_upper - rr_running_process->mem_lower - 8)
        {
            // Process has finished executing
            printf("Process %d Finished\n", rr_running_process->pid);
            set_state(rr_running_process, "Terminated"); // Set state to Terminated
            free_process(rr_running_process);
            rr_running_process = NULL;
            rr_time_slice = 0; // Reset time slice
            return;
        }

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
        rr_time_slice = 0;
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
    queue_enqueue(rr_waiting_queue[mutex_index], rr_running_process); // Enqueue the process in the waiting queue
    rr_running_process = NULL; // Clear the running process
    rr_time_slice = 0; // Reset the time slice
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

    char tmp[50];

    char *mutex_owner = getMutex(mutex_index);

    if(rr_waiting_queue[mutex_index] == NULL || queue_is_empty(rr_waiting_queue[mutex_index])) {
        printf("No processes waiting on mutex %s\n", mutex_name);
        strcpy(mutex_owner, ""); // Clear the mutex name
        return;
    }


    PCB* waiting_process = (PCB*)queue_dequeue(rr_waiting_queue[mutex_index]); // Dequeue the first waiting process
    set_state(waiting_process, "Ready"); // Set its state to Ready
    // Enqueue it in its respective priority queue
    queue_enqueue(rr_ready_queue, waiting_process);
    sprintf(tmp, "%d", waiting_process->pid);
    strcpy(mutex_owner, tmp); // Copy the process ID to the mutex name
}

Queue* rr_get_process_queue() {
    Queue* process_queue = queue_create();
    if(rr_running_process != NULL) {
        queue_enqueue(process_queue, rr_running_process);
    }
    Queue* tmp = queue_create();
    while(!queue_is_empty(rr_ready_queue)){
        PCB* process = queue_dequeue(rr_ready_queue);
        process->tiq++;
        queue_enqueue(tmp, process);
    }
    while(!queue_is_empty(tmp)){
        PCB* process = queue_dequeue(tmp);
        PCB* p1 = process;
        queue_enqueue(rr_ready_queue, process);
        queue_enqueue(process_queue, p1);
    }

    for(int i=0; i<3; i++){
        Queue* tmp = queue_create();
        while(!queue_is_empty(rr_waiting_queue[i])){
            PCB* process = queue_dequeue(rr_waiting_queue[i]);
            process->tiqblock++;
            queue_enqueue(tmp, process);
        }
        while(!queue_is_empty(tmp)){
            PCB* process = queue_dequeue(tmp);
            PCB* p1 = process;
            queue_enqueue(rr_waiting_queue[i], process);
            queue_enqueue(process_queue, p1);
        }
    }

    return process_queue; // Return the process queue
}
Queue* rr_get_ready_queue() {
    return rr_ready_queue; // Return the process queue
}
Queue* rr_get_block_queue() {
    Queue* block_queue = queue_create();
    for(int i=0; i<3; i++){
        Queue* tmp = queue_create();
        while(!queue_is_empty(rr_waiting_queue[i])){
            PCB* process = queue_dequeue(rr_waiting_queue[i]);
            queue_enqueue(tmp, process);
        }
        while(!queue_is_empty(tmp)){
            PCB* process = queue_dequeue(tmp);
            PCB* p1 = process;
            queue_enqueue(rr_waiting_queue[i], process);
            queue_enqueue(block_queue, p1);
        }
    }
    return block_queue; // Return the process queue
}
Queue* rr_get_run_queue() {
    Queue* running = queue_create();
    queue_enqueue(running,rr_running_process);
    return running; // Return the process queue
}

void set_rr_quantum(int quanta){
    char yarab[50];
    char output[50] = "Selected Quanta: ";
    rr_time_quantum = quanta;
    sprintf(yarab, "%d", rr_time_quantum);
    strcat(output, yarab);
    show_error_message(output);
}