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

<<<<<<< HEAD
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

=======
>>>>>>> zoair
int main()
{
    initialize_mem();

    // Step 1: Choose Scheduler
    char scheduler_choice[10];
    printf("Choose a scheduler (fcfs, mlfq, rr): ");
    if (scanf("%9s", scheduler_choice) != 1)
    {
        printf("Invalid input! Exiting...\n");
        return 1;
    }

    if (strcmp(scheduler_choice, "fcfs") == 0)
        set_scheduler("fcfs");
    else if (strcmp(scheduler_choice, "mlfq") == 0)
        set_scheduler("mlfq");
    else if (strcmp(scheduler_choice, "rr") == 0)
        set_scheduler("rr");
    else
    {
        printf("Invalid scheduler choice! Exiting...\n");
        return 1;
    }

    // Step 2: Choose Clock Type
    int use_automatic_clock;
    printf("Choose clock type (0 for Manual, 1 for Automatic): ");
    if (scanf("%d", &use_automatic_clock) != 1 || (use_automatic_clock != 0 && use_automatic_clock != 1))
    {
        printf("Invalid input! Exiting...\n");
        return 1;
    }

    // Step 3: Initialize Clock
    AutomaticClock *auto_clock = NULL;
    ManualClock *manual_clock = NULL;
    if (use_automatic_clock)
    {
        auto_clock = AutomaticClock_create(0.5); // 0.5 seconds interval (2 Hz)
        AutomaticClock_start(auto_clock);
    }
    else
    {
        manual_clock = ManualClock_create();
    }

    // Step 4: Load Programs
    PCB *programs[3];
    programs[0] = load_program("Program_1.txt", 1, 0);   // Arrives at cycle 0
    programs[1] = load_program("Program_2.txt", 2, 5);   // Arrives at cycle 5
    programs[2] = load_program("Program_3.txt", 3, 10);  // Arrives at cycle 10

    int added[3] = {0, 0, 0}; // Flags to check if the program has been added

    // Step 5: Run Simulation Loop
    while (1)
    {
        int current_cycle;

        if (use_automatic_clock)
        {
            current_cycle = AutomaticClock_getCycle(auto_clock);
            AutomaticClock_update(auto_clock);  // Update the clock (auto-increment the cycle)
            printf("Cycle %d - Automatic Clock\n", current_cycle);
        }
        else
        {
            ManualClock_tick(manual_clock);  // Wait for user input to increment cycle
            current_cycle = ManualClock_getCycle(manual_clock);
            printf("Cycle %d - Manual Clock\n", current_cycle);
        }

        // Check for program arrivals and add them to the scheduler
        for (int i = 0; i < 3; i++)
        {
            if (!added[i] && programs[i]->arrival_time <= current_cycle)
            {
                add_process_to_scheduler(programs[i]);
                added[i] = 1;
                printf("Program %d added to scheduler at cycle %d\n", programs[i]->pid, current_cycle);
            }
        }

        // Run the scheduler for one cycle
        run_scheduler(); 
    }

    // Cleanup (never reached due to infinite loop)
    if (auto_clock)
        AutomaticClock_destroy(auto_clock);
    if (manual_clock)
        ManualClock_destroy(manual_clock);

    return 0;
}
