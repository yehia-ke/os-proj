#include "../pcb/pcb.h"
#include "../scheduler/scheduler.h"
#include "mutex.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../popup/popup.h"
#include "../error/error.h"

// Global Mutexes defined in mutex.c
extern Mutex userInput;
extern Mutex userOutput;
extern Mutex file;

char *get_word(int index)
{
    char *word = get_memory_word(index);
    return strchr(word, ':') ? strchr(word, ':') + 1 : NULL;
}
void execute_instruction(char *instruction, PCB *process)
{
    char command[50], arg1[50], arg2[50];
    sscanf(instruction, "%s %s %s", command, arg1, arg2);

    if (strcmp(command, "semWait") == 0)
    {
        Mutex *target_mutex = NULL;

        if (strcmp(arg1, "userInput") == 0)
            target_mutex = &userInput;
        else if (strcmp(arg1, "userOutput") == 0)
            target_mutex = &userOutput;
        else if (strcmp(arg1, "file") == 0)
            target_mutex = &file;

        if (target_mutex)
        {
            if (is_owner(target_mutex, process))
            {
                printf("Mutex %s acquired by process %d\n", arg1, process->pid);
            }
            else
            {
                printf("Process %d waiting for mutex %s.\n Putting process %d in waiting queue",
                       process->pid, arg1, process->pid);

                scheduler_wait(arg1); // Wait for the mutex to be released
            }
        }
    }
    else if (strcmp(command, "semSignal") == 0)
    {
        Mutex *target_mutex = NULL;

        if (strcmp(arg1, "userInput") == 0)
            target_mutex = &userInput;
        else if (strcmp(arg1, "userOutput") == 0)
            target_mutex = &userOutput;
        else if (strcmp(arg1, "file") == 0)
            target_mutex = &file;

        if (target_mutex)
        {
            char tmp[50];
            sprintf(tmp, "%d", process->pid); // Format PCB pid as a string

            // Check if the current process is the owner
            if (strcmp(target_mutex->owner, tmp) == 0)
            {
                memset(target_mutex->owner, 0, sizeof(target_mutex->owner)); // Clear the owner
                printf("Mutex %s released by process %d\n", arg1, process->pid);
                scheduler_signal(arg1); // Signal the mutex
            }
            else
            {
                printf("Process %d cannot release mutex %s as it is not the owner\n", process->pid, arg1);
            }
        }
    }
    else if (strcmp(command, "assign") == 0)
    {
        if (strcmp(arg1, "a") == 0 && strcmp(arg2, "input") == 0)
        {
            // Assign user input to var1
            printf("Please enter a value for a: ");
            char *value = show_text_entry_popup("Enter a value for a");
            // int c;
            // while ((c = getchar()) != '\n' && c != EOF)
            //     ;
            // fgets(value, sizeof(value), stdin);
            // value[strcspn(value, "\n")] = '\0';
            printf("%s\n", value);
            set_variable(process, "var1", value);
        }
        else if (strcmp(arg1, "b") == 0 && strcmp(arg2, "input") == 0)
        {
            // Assign user input to var2
            printf("Please enter a value for b: ");
            char *value = show_text_entry_popup("Enter a value for b");
            // int c;
            // fgets(value, sizeof(value), stdin);
            // value[strcspn(value, "\n")] = '\0';
            printf("%s\n", value);
            set_variable(process, "var2", value);
        }
        else if (strcmp(arg1, "b") == 0 && strncmp(arg2, "readFile", 8) == 0)
        {
            // Assign the result of readFile to b
            char *filename = get_word(process->mem_lower + 6); // var1
            FILE *fp = fopen(filename, "r");
            if (fp)
            {
                char buffer[256] = {0};
                ;
                fread(buffer, sizeof(char), 255, fp);
                buffer[255] = '\0'; // Ensure null-terminated string
                fclose(fp);
                set_variable(process, "var2", buffer); // Store in var2
                printf("Data read from file %s\n", filename);
            }
            else
            {
                printf("Error: Could not open file %s for reading\n", filename);
            }
        }
        else if (strcmp(arg1, "a") == 0 && strcmp(arg2, "b") == 0)
        {
            // Assign value of var2 to var1
            char *value = get_word(process->mem_lower + 7); // var2
            set_variable(process, "var1", value);
        }
        else if (strcmp(arg1, "b") == 0 && strcmp(arg2, "a") == 0)
        {
            // Assign value of var1 to var2
            char *value = get_word(process->mem_lower + 6); // var1
            set_variable(process, "var2", value);
        }
        else
        {
            set_variable(process, arg1, arg2);
        }
    }
    else if (strcmp(command, "print") == 0)
{
    char *value = NULL;

    if (strcmp(arg1, "a") == 0)
    {
        value = get_word(process->mem_lower + 6); // var1
    }
    else if (strcmp(arg1, "b") == 0)
    {
        value = get_word(process->mem_lower + 7); // var2
    }
    else if (strcmp(arg1, "c") == 0)
    {
        value = get_word(process->mem_lower + 8); // var3
    }

    if (value)
    {
        char output[256] = "Output: "; // Allocate a writable buffer with sufficient size

        strncat(output, value, sizeof(output) - strlen(output) - 1); // Safely concatenate
        printf("%s\n", output); // Print the concatenated output

        // Assuming `show_info_message` function exists for informational messages
        show_error_message(output); 
    }
    else
    {
        printf("Error: Invalid or missing value for argument '%s'\n", arg1);
    }
}
    else if (strcmp(command, "printFromTo") == 0)
    {
        int start = atoi(get_word(process->mem_lower + 6)); // var1
        int end = atoi(get_word(process->mem_lower + 7));   // var2
        char output[1024] = "Output: ";                     // Allocate a writable buffer with sufficient size
        char buffer[16];                                    // Temporary buffer for integer-to-string conversion

        if (start > end)
        {
            for (int i = end; i <= start; i++)
            {
                sprintf(buffer, "%d", i); // Convert integer to string
                strcat(output, buffer);   // Append the number to the output
                strcat(output, " ");      // Append a space
                printf("%d ", i);         // Print the number
            }
        }
        else
        {
            for (int i = start; i <= end; i++)
            {
                sprintf(buffer, "%d", i); // Convert integer to string
                strcat(output, buffer);   // Append the number to the output
                strcat(output, " ");      // Append a space
                printf("%d ", i);         // Print the number
            }
        }
        printf("\n");
        show_error_message(output); // Assuming this function logs informational messages
    }
    else if (strcmp(command, "writeFile") == 0)
    {
        char *filename = get_word(process->mem_lower + 6); // var1
        char *data = get_word(process->mem_lower + 7);     // var2
        FILE *fp = fopen(filename, "w");
        if (fp)
        {
            fprintf(fp, "%s", data);
            fclose(fp);
            printf("Data written to file %s\n", filename);
        }
        else
        {
            printf("Error: Could not open file %s for writing\n", filename);
        }
    }
    else if (strcmp(command, "exit") == 0)
    {
        printf("Process %d exiting...\n", process->pid);
        free_process(process);
    }
    else
    {
        printf("Unknown command: %s\n", command);
    }
}