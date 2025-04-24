#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcb/pcb.h"
#include "interpreter/interpreter.h"
#include "scheduler/scheduler.h"
#include "AutomaticClock/AutomaticClock.h"
#include "ManualClock/ManualClock.h"

// Function to load a program from a file into a PCB
PCB *load_program(const char *filename, int pid, int arrival_time)
{
    PCB *process = create_process(pid, (char *)filename, arrival_time);
    if (!process)
    {
        printf("Failed to load program: %s\n", filename);
        exit(1);
    }
    return process;
}

// Function to simulate the execution of a process
void execute_program(PCB *process)
{
    printf("\n--- Executing Process %d ---\n", process->pid);
    char *instruction = NULL;

    while ((instruction = get_instruction(process)) != NULL)
    {
        printf("Executing: %s\n", instruction);
        execute_instruction(instruction, process);
        increment_pc(process); // Advance the program counter
    }
    printf("--- Process %d Execution Complete ---\n", process->pid);
    free_process(process);
}

int main()
{
    // Step 1: Choose Scheduler
    initialize_mem();
    char scheduler_choice[10];
    printf("Choose a scheduler (fcfs, mlfq, rr): ");
    if (scanf("%9s", scheduler_choice) != 1)
    {
        printf("Invalid input! Exiting...\n");
        return 1;
    }

    if (strcmp(scheduler_choice, "fcfs") == 0)
    {
        set_scheduler("fcfs");
    }
    else if (strcmp(scheduler_choice, "mlfq") == 0)
    {
        set_scheduler("mlfq");
    }
    else if (strcmp(scheduler_choice, "rr") == 0)
    {
        set_scheduler("rr");
    }
    else
    {
        printf("Invalid scheduler choice! Exiting...\n");
        return 1;
    }

    // Step 2: Choose Clock Type (Automatic or Manual)
    int use_automatic_clock;
    printf("Choose clock type (0 for Manual, 1 for Automatic): ");
    if (scanf("%d", &use_automatic_clock) != 1 || (use_automatic_clock != 0 && use_automatic_clock != 1))
    {
        printf("Invalid input! Exiting...\n");
        return 1;
    }

    // Step 3: Initialize Clock
    AutomaticClock* auto_clock = NULL;
    ManualClock* manual_clock = NULL;
    if (use_automatic_clock)
    {
        auto_clock = AutomaticClock_create(10.0); // 10 Hz Frequency for automatic clock
        AutomaticClock_start(auto_clock);
    }
    else
    {
        manual_clock = ManualClock_create();
    }

    // Step 4: Choose Program to Run
    int program_choice;

    // Clear input buffer after the first scanf
    while (getchar() != '\n'); // Consume leftover characters from the input buffer

    printf("\nAvailable Programs:\n");
    printf("1. Program 1\n");
    printf("2. Program 2\n");
    printf("3. Program 3\n");
    printf("Enter the program number you want to execute: ");
    if (scanf("%d", &program_choice) != 1)
    {
        printf("Invalid input! Please enter a valid program number.\n");
        return 1;
    }

    PCB *selected_program = NULL;

    switch (program_choice)
    {
    case 1:
        selected_program = load_program("Program_1.txt", 1, 0);
        break;
    case 2:
        selected_program = load_program("Program_2.txt", 2, 1);
        break;
    case 3:
        selected_program = load_program("Program_3.txt", 3, 2);
        break;
    default:
        printf("Invalid program choice! Exiting...\n");
        return 1;
    }

    add_process_to_scheduler(selected_program);
    
    // Simulate the process running, either automatically or manually
    for (int cycle = 0; cycle < 10; ++cycle)
    {
        if (use_automatic_clock && auto_clock)
        {
            // Automatic clock updates and prints PCB every cycle
            double current_time = cycle * 0.1; // Example, assuming 0.1s per cycle
            AutomaticClock_update(auto_clock, current_time);
            printf("Cycle %d - Automatic Clock: %d\n", cycle, AutomaticClock_getCycle(auto_clock));
        }
        else if (manual_clock)
        {
            // Manual clock ticks each time you decide
            ManualClock_tick(manual_clock);
            printf("Cycle %d - Manual Clock: %d\n", cycle, ManualClock_getCycle(manual_clock));
        }

        //printPCB(selected_program); // Print the current PCB state
        run_scheduler();
    }

    // Clean up
    if (auto_clock)
    {
        AutomaticClock_destroy(auto_clock);
    }
    if (manual_clock)
    {
        ManualClock_destroy(manual_clock);
    }

    return 0;
}
