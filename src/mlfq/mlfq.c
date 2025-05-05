#include "mlfq.h"
#include <stdio.h>

PCB *mlfq_running_process = NULL;   // Pointer to the currently running process
Queue *mlfq_ready_queue[4];         // Array of queues for each priority level
PQueue *mlfq_waiting_queue[3];      // Array of queues for processes waiting for mutex variable i
int time_quantum[4] = {1, 2, 4, 8}; // Time quantum for each priority level

int time_slice = 0; // Time slice counter

void initialize_mlfq()
{
    for (int i = 0; i < 4; i++)
    {
        if (i < 3)
        {
            mlfq_waiting_queue[i] = pqueue_create(); // Initialize each waiting queue
        }
        mlfq_ready_queue[i] = queue_create(); // Initialize each priority queue
    }
}

void mlfq_add_process(PCB *process)
{
    // Add the process to the highest priority queue (0)
    queue_enqueue(mlfq_ready_queue[0], process);
}

void mlfq_wait(char mutex_name[])
{
    // Add the process to the waiting queue

    int mutex_index;

    if (strcmp(mutex_name, "userInput") == 0)
    {
        mutex_index = 0;
    }
    else if (strcmp(mutex_name, "userOutput") == 0)
    {
        mutex_index = 1;
    }
    else if (strcmp(mutex_name, "file") == 0)
    {
        mutex_index = 2;
    }
    else
    {
        printf("Error: Unknown mutex name %s\n", mutex_name);
        return;
    }

    set_state(mlfq_running_process, "Waiting");                            // Set the state to Waiting
    if(time_slice >= time_quantum[mlfq_running_process->priority - 1])
    {
        set_priority(mlfq_running_process, mlfq_running_process->priority + 1); // Decrease the priority
    }
    pqueue_enqueue(mlfq_waiting_queue[mutex_index], mlfq_running_process); // Enqueue the process in the waiting queue
    mlfq_running_process = NULL;                                           // Clear the running process
    time_slice = 0;                                                        // Reset the time slice
}

void mlfq_signal(char mutex_name[])
{
    // Signal the mutex and wake up the waiting process
    int mutex_index;

    if (strcmp(mutex_name, "userInput") == 0)
    {
        mutex_index = 0;
    }
    else if (strcmp(mutex_name, "userOutput") == 0)
    {
        mutex_index = 1;
    }
    else if (strcmp(mutex_name, "file") == 0)
    {
        mutex_index = 2;
    }
    else
    {
        printf("Error: Unknown mutex name %s\n", mutex_name);
        return;
    }

    char tmp[50];

    char *mutex_owner = getMutex(mutex_index);

    if (mlfq_waiting_queue[mutex_index] == NULL || pqueue_is_empty(mlfq_waiting_queue[mutex_index]))
    {
        printf("No processes waiting on mutex %s\n", mutex_name);
        strcpy(mutex_owner, ""); // Clear the mutex name
        return;
    }

    PCB *waiting_process = (PCB *)pqueue_dequeue(mlfq_waiting_queue[mutex_index]); // Dequeue the first waiting process
    // Enqueue it in its respective priority queue
    queue_enqueue(mlfq_ready_queue[waiting_process->priority - 1], waiting_process);
    sprintf(tmp, "%d", waiting_process->pid);
    strcpy(mutex_owner, tmp);            // Copy the process ID to the mutex name
    set_state(waiting_process, "Ready"); // Set its state to Ready
}

void run_mlfq()
{
    if (mlfq_running_process != NULL)
    {
        // Get the next instruction from the running process
        char *instruction = get_instruction(mlfq_running_process);
        if (instruction == NULL)
        {
            printf("No instruction to run... Terminating\n");
            set_state(mlfq_running_process, "Terminated"); // Set state to Terminated
            free_process(mlfq_running_process);
            mlfq_running_process = NULL;
            time_slice = 0; // Reset time slice
            return;
        }
        // If there's still an instruction, return it and increment PC
        printf("Process %d running: %s\n", mlfq_running_process->pid, instruction);
        increment_pc(mlfq_running_process);
        time_slice++;
        execute_instruction(instruction, mlfq_running_process); // Execute the instruction

        if (!mlfq_running_process) // Checks if the process has been blocked by a mutex
        {
            time_slice = 0; // Reset time slice
            return;
        }

        if(mlfq_running_process->pc == mlfq_running_process->mem_upper - mlfq_running_process->mem_lower - 8)
        {
            // Process has finished executing
            printf("Process %d Finished\n", mlfq_running_process->pid);
            set_state(mlfq_running_process, "Terminated"); // Set state to Terminated
            free_process(mlfq_running_process);
            mlfq_running_process = NULL;
            time_slice = 0; // Reset time slice
            return;
        }

        if (time_slice >= time_quantum[mlfq_running_process->priority - 1])
        {
            // Time slice expired, move to the next queue
            printf("Quantum expired for process %d\n", mlfq_running_process->pid);
            mlfq_running_process->tiq--;
            set_state(mlfq_running_process, "Ready");
            if (mlfq_running_process->priority <= 3)
            {
                // Decrease the priority
                set_priority(mlfq_running_process, mlfq_running_process->priority + 1);
                // Enqueue it in the next lower priority queue
                queue_enqueue(mlfq_ready_queue[mlfq_running_process->priority - 1], mlfq_running_process);
            }
            else
            {
                // If it's the lowest priority, just requeue it
                queue_enqueue(mlfq_ready_queue[3], mlfq_running_process);
            }
            mlfq_running_process = NULL;
            time_slice = 0; // Reset time slice
        }
        return; // Exit the function to avoid running the next process
    }

    int current_priority = 0; // Current priority level

    // If no process is running, check if the highest priority queue is empty
    if (!queue_is_empty(mlfq_ready_queue[0]))
    {
        current_priority = 0; // Reset current priority
    }
    else if (!queue_is_empty(mlfq_ready_queue[1]))
    {
        current_priority = 1; // Set to next priority
    }
    else if (!queue_is_empty(mlfq_ready_queue[2]))
    {
        current_priority = 2; // Set to next priority
    }
    else if (!queue_is_empty(mlfq_ready_queue[3]))
    {
        current_priority = 3; // Set to next priority
    }
    else
    {
        printf("No processes in any queue\n");
        return; // No processes in any queue
    }

    // If the highest priority queue isn't empty, start the next process
    mlfq_running_process = (PCB *)queue_dequeue(mlfq_ready_queue[current_priority]); // Get the next process
    set_state(mlfq_running_process, "Running");                                      // Set its state to running
    printf("Process %d is starting\n", mlfq_running_process->pid);

    // Get its first instruction
    char *instruction = get_instruction(mlfq_running_process);
    if (instruction != NULL)
    {
        printf("Process %d running: %s\n", mlfq_running_process->pid, instruction);
        increment_pc(mlfq_running_process);
        time_slice++;
        execute_instruction(instruction, mlfq_running_process); // Execute the instruction

        if (!mlfq_running_process) // Checks if the process has been blocked by a mutex
        {
            time_slice = 0; // Reset time slice
            return;
        }

        if(mlfq_running_process->pc == mlfq_running_process->mem_upper - mlfq_running_process->mem_lower - 8)
        {
            // Process has finished executing
            printf("Process %d Finished\n", mlfq_running_process->pid);
            set_state(mlfq_running_process, "Terminated"); // Set state to Terminated
            free_process(mlfq_running_process);
            mlfq_running_process = NULL;
            time_slice = 0; // Reset time slice
            return;
        }

        if (time_slice >= time_quantum[mlfq_running_process->priority - 1])
        {
            // Time slice expired, move to the next queue
            printf("Quantum expired for process %d\n", mlfq_running_process->pid);
            mlfq_running_process->tiq--;
            set_state(mlfq_running_process, "Ready");
            if (mlfq_running_process->priority <= 3)
            {
                // Decrease the priority
                set_priority(mlfq_running_process, mlfq_running_process->priority + 1);
                // Enqueue it in the next lower priority queue
                queue_enqueue(mlfq_ready_queue[mlfq_running_process->priority - 1], mlfq_running_process);
            }
            else
            {
                // If it's the lowest priority, just requeue it
                queue_enqueue(mlfq_ready_queue[3], mlfq_running_process);
            }
            mlfq_running_process = NULL;
            time_slice = 0; // Reset time slice
        }
    }
    else
    {
        printf("No instruction to run... Terminating\n");
        set_state(mlfq_running_process, "Terminated"); // Set state to Terminated
        free_process(mlfq_running_process);
        mlfq_running_process = NULL;
        time_slice = 0;
        return;
    }
}

Queue *mlfq_get_process_queue()
{
    Queue *process_queue = queue_create();
    if (mlfq_running_process != NULL)
    {
        queue_enqueue(process_queue, mlfq_running_process);
    }
    for (int i = 0; i < 4; i++)
    {
        Queue *tmp = queue_create();
        while (!queue_is_empty(mlfq_ready_queue[i]))
        {
            PCB *process = queue_dequeue(mlfq_ready_queue[i]);
            process->tiq++;
            queue_enqueue(tmp, process);
        }
        while (!queue_is_empty(tmp))
        {
            PCB *process = queue_dequeue(tmp);
            PCB *p1 = process;
            queue_enqueue(mlfq_ready_queue[i], process);
            queue_enqueue(process_queue, p1);
        }
    }

    for (int i = 0; i < 3; i++)
    {
        Queue *tmp = queue_create();
        while (!pqueue_is_empty(mlfq_waiting_queue[i]))
        {
            PCB *process = pqueue_dequeue(mlfq_waiting_queue[i]);
            process->tiqblock++;
            queue_enqueue(tmp, process);
        }
        while (!queue_is_empty(tmp))
        {
            PCB *process = queue_dequeue(tmp);
            PCB *p1 = process;
            pqueue_enqueue(mlfq_waiting_queue[i], process);
            queue_enqueue(process_queue, p1);
        }
    }

    return process_queue; // Return the process queue
}
Queue *mlfq_get_ready_queue()
{
    Queue *ready_queue = queue_create();
    for (int i = 0; i < 4; i++)
    {
        Queue *tmp = queue_create();
        while (!queue_is_empty(mlfq_ready_queue[i]))
        {
            PCB *process = queue_dequeue(mlfq_ready_queue[i]);
            queue_enqueue(tmp, process);
        }
        while (!queue_is_empty(tmp))
        {
            PCB *process = queue_dequeue(tmp);
            PCB *p1 = process;
            queue_enqueue(mlfq_ready_queue[i], process);
            queue_enqueue(ready_queue, p1);
        }
    }
    return ready_queue; // Return the process queue // Return the highest priority queue
}
Queue *mlfq_get_block_queue()
{
    Queue *block_queue = queue_create();
    for (int i = 0; i < 3; i++)
    {
        Queue *tmp = queue_create();
        while (!pqueue_is_empty(mlfq_waiting_queue[i]))
        {
            PCB *process = pqueue_dequeue(mlfq_waiting_queue[i]);
            queue_enqueue(tmp, process);
        }
        while (!queue_is_empty(tmp))
        {
            PCB *process = queue_dequeue(tmp);
            PCB *p1 = process;
            pqueue_enqueue(mlfq_waiting_queue[i], process);
            queue_enqueue(block_queue, p1);
        }
    }
    return block_queue; // Return the process queue
}
Queue *mlfq_get_run_queue()
{
    Queue *running = queue_create();
    queue_enqueue(running, mlfq_running_process);

    return running; // Return the highest priority queue
}
