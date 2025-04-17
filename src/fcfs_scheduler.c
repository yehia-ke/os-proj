#include "fcfs_scheduler.h"
#include <stdio.h>
#include <windows.h> // For Sleep()

void run_fcfs(PCB* processes[], int num_processes) {  //processes[]: a list of process pointers, num_processes: how many there are
    for (int i = 0; i < num_processes; i++) {  //For-loop to go through each process in the order they were created
        PCB* process = processes[i];  //We grab the current process from the array

        printf("Running Process %d\n", process->pid);  //Show which process is being run right now
        set_state(process, "Running");  //Update the process state to "Running" in both: Memory and the PCB struct

        char* instruction;  //Declare a variable to hold each instruction
        while ((instruction = get_instruction(process)) != NULL) {  //Get the next instruction based on the process’s PC(If there's an instruction, run the loop, If there's no more, exit the loop)
            printf("Executing: %s\n", instruction);  //Print the instruction so you can see what’s running
            process->pc++;  //Move the PC to the next instruction so next time it gets the following one.
            Sleep(500);  //This makes the program wait for half a second(It’s just to simulate time passing like the CPU is executing)
        }

        set_state(process, "Terminated");  //Set the process state to "Terminated"
        printf("Process %d Finished\n\n", process->pid);  //Print that the process has finished
        free_process(process);  //Free the memory of that process (both from memory and from the heap)
    }
}
