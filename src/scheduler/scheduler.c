#include "scheduler.h"
#include <stdio.h>
#include <string.h>
#include "../queue/queue.h"
#include "../main.h"

char* current_scheduler = NULL;  // Pointer to the currently set scheduler

void set_scheduler(char *scheduler_name)
{
    if (strcmp(scheduler_name, "fcfs") == 0)
    {
        initialize_fcfs();                  
        current_scheduler = scheduler_name; 
        printf("Scheduler set to: %s\n", current_scheduler);
    } else if (strcmp(scheduler_name, "rr") == 0) {
        initialize_rr();  
        current_scheduler = scheduler_name; 
        printf("Scheduler set to: %s\n", current_scheduler);
    }
    else if (strcmp(scheduler_name, "mlfq") == 0)
    {
        initialize_mlfq();                 
        current_scheduler = scheduler_name; 
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

    if (strcmp(current_scheduler, "fcfs") == 0) {
        run_fcfs();
    } else if (strcmp(current_scheduler, "rr") == 0) {
        run_rr();
    } else if (strcmp(current_scheduler, "mlfq") == 0) {
        run_mlfq();
    }
}

void add_process_to_scheduler(PCB *process) {
    if (current_scheduler == NULL) {
        printf("No scheduler set.\n");
        return;
    }

    if (strcmp(current_scheduler, "fcfs") == 0) {
        fcfs_add_process(process);
    } else if (strcmp(current_scheduler, "rr") == 0) {
        rr_add_process(process);
    }
    else if (strcmp(current_scheduler, "mlfq") == 0) {
        mlfq_add_process(process);
    }
}

void scheduler_wait(char mutex_name[]) {
    if (current_scheduler == NULL) {
        printf("No scheduler set.\n");
        return;
    }

    // Call the appropriate scheduler's wait function
    if (strcmp(current_scheduler, "fcfs") == 0) {
        fcfs_wait(mutex_name);
    } else if (strcmp(current_scheduler, "rr") == 0) {
        rr_wait(mutex_name);
    }
    else if (strcmp(current_scheduler, "mlfq") == 0) {
        mlfq_wait(mutex_name);
    }
}

void scheduler_signal(char mutex_name[]) {
    if (current_scheduler == NULL) {
        printf("No scheduler set.\n");
        return;
    }

    if (strcmp(current_scheduler, "fcfs") == 0) {
        fcfs_signal(mutex_name);
    } else if (strcmp(current_scheduler, "rr") == 0) {
        rr_signal(mutex_name);
    }
    else if (strcmp(current_scheduler, "mlfq") == 0) {
        mlfq_signal(mutex_name);
    }
}

Queue* get_process_queue() {
    if (current_scheduler == NULL) {
        printf("No scheduler set.\n");
        return NULL;
    }
    if (strcmp(current_scheduler, "fcfs") == 0) {
        return fcfs_get_process_queue();
        
    } else if (strcmp(current_scheduler, "rr") == 0) {
        return rr_get_process_queue();
    }
    else if (strcmp(current_scheduler, "mlfq") == 0) {
        return mlfq_get_process_queue();
    }
    else{
    }
}

Queue* get_ready_queue(){
    if (current_scheduler == NULL) {
        printf("No scheduler set.\n");
        return NULL;
    }
    if (strcmp(current_scheduler, "fcfs") == 0) {
        return fcfs_get_ready_queue();
        
    } else if (strcmp(current_scheduler, "rr") == 0) {
        return rr_get_ready_queue();
    }
    else if (strcmp(current_scheduler, "mlfq") == 0) {
        return mlfq_get_ready_queue();
    }
    else{
    }
}

Queue* get_block_queue(){
    if (current_scheduler == NULL) {
        printf("No scheduler set.\n");
        return NULL;
    }
    if (strcmp(current_scheduler, "fcfs") == 0) {
        return fcfs_get_block_queue();
        
    } else if (strcmp(current_scheduler, "rr") == 0) {
        return rr_get_block_queue();
    }
    else if (strcmp(current_scheduler, "mlfq") == 0) {
        return mlfq_get_block_queue();
    }
    else{
    }
}

Queue* get_run_queue(){
    if (current_scheduler == NULL) {
        printf("No scheduler set.\n");
        return NULL;
    }
    if (strcmp(current_scheduler, "fcfs") == 0) {
        return fcfs_get_run_queue();
        
    } else if (strcmp(current_scheduler, "rr") == 0) {
        return rr_get_run_queue();
    }
    else if (strcmp(current_scheduler, "mlfq") == 0) {
        return mlfq_get_run_queue();
    }
    else{
    }
}