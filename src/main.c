#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcb/pcb.h"
#include "interpreter/interpreter.h"
#include "scheduler/scheduler.h"

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
        process->pc++; // Advance the program counter
    }
    printf("--- Process %d Execution Complete ---\n", process->pid);
    free_process(process);
}

int main()
{
    // Step 1: Choose Scheduler
    initialize_mem();
    char scheduler_choice[10];
    printf("Choose a scheduler (fcfs, mlfq): ");
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
    else
    {
        printf("Invalid scheduler choice! Exiting...\n");
        return 1;
    }

    // Step 2: Choose Program to Run
    int program_choice;

    // Clear input buffer after the first scanf
    while (getchar() != '\n'); // Consume leftover characters from the input buffer

    printf("\nAvailable Programs:\n");
    printf("1. Program 1\n");
    printf("2. Program 2\n");
    printf("3. Program 3\n");
    printf("Enter the program number you want to execute: ");
    if (scanf("%d", &program_choice) != 1) { // Check if scanf reads an integer
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

    // Step 3: Execute the Selected Program
    execute_program(selected_program);
    return 0;
}