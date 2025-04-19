#include "../pcb/pcb.h"
#include "mutex.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Global Mutexes defined in mutex.c
extern Mutex userInput;
extern Mutex userOutput;
extern Mutex file;

void execute_instruction(char *instruction, PCB *process) {
    char command[50], arg1[50], arg2[50];
    sscanf(instruction, "%s %s %s", command, arg1, arg2);

    if (strcmp(command, "semWait") == 0) {
        Mutex *target_mutex = NULL;

        if (strcmp(arg1, "userInput") == 0) target_mutex = &userInput;
        else if (strcmp(arg1, "userOutput") == 0) target_mutex = &userOutput;
        else if (strcmp(arg1, "file") == 0) target_mutex = &file;

        if (target_mutex && is_owner(target_mutex, process)) {
            printf("Mutex %s acquired by process %d\n", arg1, process->pid);
        } else {
            printf("Process %d waiting for mutex %s\n", process->pid, arg1);
        }
    }
    else if (strcmp(command, "semSignal") == 0) {
        Mutex *target_mutex = NULL;

        if (strcmp(arg1, "userInput") == 0) target_mutex = &userInput;
        else if (strcmp(arg1, "userOutput") == 0) target_mutex = &userOutput;
        else if (strcmp(arg1, "file") == 0) target_mutex = &file;

        if (target_mutex && strcmp(target_mutex->owner, getPID(process)) == 0) {
            memset(target_mutex->owner, 0, sizeof(target_mutex->owner));
            printf("Mutex %s released by process %d\n", arg1, process->pid);
        } else {
            printf("Process %d cannot release mutex %s as it is not the owner\n", process->pid, arg1);
        }
    }
    else if (strcmp(command, "assign") == 0) {
        if (strcmp(arg2, "input") == 0) {
            printf("Please enter a value for %s: ", arg1);
            char value[50];
            scanf("%s", value);
            set_variable(process, arg1, value);
        } else {
            set_variable(process, arg1, arg2);
        }
    }
    else if (strcmp(command, "print") == 0) {
        char *value = NULL;

        if (strcmp(arg1, "var1") == 0) {
            value = get_memory_word(process->mem_lower + 6); // var1
        } else if (strcmp(arg1, "var2") == 0) {
            value = get_memory_word(process->mem_lower + 7); // var2
        } else if (strcmp(arg1, "var3") == 0) {
            value = get_memory_word(process->mem_lower + 8); // var3;
        }

        if (value) {
            printf("Output: %s\n", value);
        }
    }
    else if (strcmp(command, "printFromTo") == 0) {
        int start = atoi(get_memory_word(process->mem_lower + 6)); // var1
        int end = atoi(get_memory_word(process->mem_lower + 7));   // var2
        printf("Output: ");
        for (int i = start; i <= end; i++) {
            printf("%d ", i);
        }
        printf("\n");
    }
    else if (strcmp(command, "writeFile") == 0) {
        char *filename = get_memory_word(process->mem_lower + 6); // var1
        char *data = get_memory_word(process->mem_lower + 7);     // var2
        FILE *fp = fopen(filename, "w");
        if (fp) {
            fprintf(fp, "%s", data);
            fclose(fp);
            printf("Data written to file %s\n", filename);
        } else {
            printf("Error: Could not open file %s for writing\n", filename);
        }
    }
    else if (strcmp(command, "readFile") == 0) {
        char *filename = get_memory_word(process->mem_lower + 6); // var1
        FILE *fp = fopen(filename, "r");
        if (fp) {
            char buffer[256];
            fread(buffer, sizeof(char), 255, fp);
            buffer[255] = '\0'; // Ensure null-terminated string
            fclose(fp);
            set_variable(process, "var2", buffer); // Store in var2
            printf("Data read from file %s\n", filename);
        } else {
            printf("Error: Could not open file %s for reading\n", filename);
        }
    }
}