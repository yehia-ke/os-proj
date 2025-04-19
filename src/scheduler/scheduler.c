#include "scheduler.h"
#include <stdio.h>

char* current_scheduler = NULL;  // Pointer to the currently set scheduler

void set_scheduler(char *scheduler_name)
{
    if (strcmp(scheduler_name, "fcfs") == 0)
    {
        initialize_fcfs();                  // Initialize mlfq scheduler
        current_scheduler = scheduler_name; // Set the current scheduler
        printf("Scheduler set to: %s\n", current_scheduler);
    } /*else if (strcmp(scheduler_name, "rr") == 0) {
        initialize_rr();  // Initialize rr scheduler
        current_scheduler = scheduler_name;  // Set the current scheduler
        printf("Scheduler set to: %s\n", current_scheduler);
    }*/
    else if (strcmp(scheduler_name, "mlfq") == 0)
    {
        initialize_mlfq();                  // Initialize mlfq scheduler
        current_scheduler = scheduler_name; // Set the current scheduler
        printf("Scheduler set to: %s\n", current_scheduler);
    }
    else
    {
        printf("Unknown scheduler: %s\n", current_scheduler);
        return;
    }
}

char* run_scheduler() {
    if (current_scheduler == NULL) {
        printf("No scheduler set.\n");
        return NULL;
    }

    // Call the appropriate scheduler's run function
    if (strcmp(current_scheduler, "fcfs") == 0) {
        run_fcfs();
    } else if (strcmp(current_scheduler, "rr") == 0) {
        run_rr();
    } else if (strcmp(current_scheduler, "mlfq") == 0) {
        run_mlfq();
    } else {
        printf("Unknown scheduler: %s\n", current_scheduler);
        return NULL;
    }
}

void add_process_to_scheduler(PCB *process) {
    if (current_scheduler == NULL) {
        printf("No scheduler set.\n");
        return;
    }

    // Call the appropriate scheduler's add process function
    if (strcmp(current_scheduler, "fcfs") == 0) {
        fcfs_add_process(process);
    } /*else if (strcmp(current_scheduler, "rr") == 0) {
        rr_add_process(process);
    }*/
    else if (strcmp(current_scheduler, "mlfq") == 0) {
        mlfq_add_process(process);
    } else {
        printf("Unknown scheduler: %s\n", current_scheduler);
        return;
    }
}