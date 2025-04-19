#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#define PROGRAMS_FOLDER "../../programs/" // Define the folder where programs are stored
#define MAX_VARIABLES 100 // Maximum number of variables that can be stored
#define MAX_FILE_CONTENT 1024 // Maximum size for file content

// Mutexes for mutual exclusion
pthread_mutex_t userInputMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t userOutputMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;

// Variable storage
typedef struct {
    char name[50];
    char value[1024]; // Store values as strings to handle both text and numbers
} Variable;

Variable variables[MAX_VARIABLES];
int variableCount = 0;

// Function to get the value of a variable
int getVariableValue(const char *name, char *value) {
    for (int i = 0; i < variableCount; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            strcpy(value, variables[i].value);
            return 1; // Found
        }
    }
    return 0; // Not found
}

// Function to set or update a variable's value
void setVariableValue(const char *name, const char *value) {
    for (int i = 0; i < variableCount; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            strcpy(variables[i].value, value);
            return;
        }
    }
    // If the variable doesn't exist, add it
    if (variableCount < MAX_VARIABLES) {
        strcpy(variables[variableCount].name, name);
        strcpy(variables[variableCount].value, value);
        variableCount++;
    } else {
        printf("Error: Maximum variable limit reached.\n");
    }
}

// Function to handle 'printFromTo'
void printFromTo(const char *startVariable, const char *endVariable) {
    int startValue, endValue;
    char resolvedStart[1024], resolvedEnd[1024];

    // Resolve start value
    if (getVariableValue(startVariable, resolvedStart)) {
        startValue = atoi(resolvedStart);
    } else {
        startValue = atoi(startVariable); // Fallback to direct value
    }

    // Resolve end value
    if (getVariableValue(endVariable, resolvedEnd)) {
        endValue = atoi(resolvedEnd);
    } else {
        endValue = atoi(endVariable); // Fallback to direct value
    }

    // Print values from start to end
    if (startValue <= endValue) {
        printf("Printing numbers from %d to %d:\n", startValue, endValue);
        for (int i = startValue; i <= endValue; i++) {
            printf("%d\n", i);
        }
    } else {
        printf("Error: Invalid range. Start (%d) is greater than end (%d).\n", startValue, endValue);
    }
}

// Function to handle 'writeFile'
void writeFile(const char *filenameVariable, const char *dataVariable) {
    char resolvedFilename[1024];
    char resolvedData[1024];

    // Resolve filename
    if (!getVariableValue(filenameVariable, resolvedFilename)) {
        printf("[ERROR] Variable '%s' not found. Cannot resolve filename.\n", filenameVariable);
        return;
    }

    // Resolve data
    if (!getVariableValue(dataVariable, resolvedData)) {
        printf("[ERROR] Variable '%s' not found. Cannot resolve data.\n", dataVariable);
        return;
    }

    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s%s", PROGRAMS_FOLDER, resolvedFilename);

    // Check if file already exists
    FILE *file = fopen(fullPath, "r");
    if (file != NULL) {
        fclose(file);
        printf("[ERROR] File '%s' already exists. Operation aborted to prevent overwriting.\n", fullPath);
        return;
    }

    // Write to the file
    file = fopen(fullPath, "w");
    if (file == NULL) {
        perror("Error writing to file");
        return;
    }

    fprintf(file, "%s", resolvedData);
    fclose(file);

    printf("[INFO] Successfully wrote to file '%s'.\n", fullPath);
}

// Function to handle 'readFile'
char *readFile(const char *filename) {
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s%s", PROGRAMS_FOLDER, filename);

    FILE *file = fopen(fullPath, "r");
    if (file == NULL) {
        perror("Error reading file");
        return NULL;
    }

    char *content = (char *)malloc((MAX_FILE_CONTENT + 1) * sizeof(char));
    if (content == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(content, sizeof(char), MAX_FILE_CONTENT, file);
    content[bytesRead] = '\0';
    fclose(file);

    // Print the file contents
    printf("[INFO] File Contents of '%s':\n%s\n", filename, content);

    return content;
}

// Function to parse and execute a single command
void executeCommand(char *command) {
    char operation[50];
    sscanf(command, "%s", operation);

    if (strcmp(operation, "semWait") == 0) {
        char resource[50];
        sscanf(command, "semWait %s", resource);
        if (strcmp(resource, "userInput") == 0) {
            pthread_mutex_lock(&userInputMutex);
        } else if (strcmp(resource, "userOutput") == 0) {
            pthread_mutex_lock(&userOutputMutex);
        } else if (strcmp(resource, "file") == 0) {
            pthread_mutex_lock(&fileMutex);
        }
    } else if (strcmp(operation, "semSignal") == 0) {
        char resource[50];
        sscanf(command, "semSignal %s", resource);
        if (strcmp(resource, "userInput") == 0) {
            pthread_mutex_unlock(&userInputMutex);
        } else if (strcmp(resource, "userOutput") == 0) {
            pthread_mutex_unlock(&userOutputMutex);
        } else if (strcmp(resource, "file") == 0) {
            pthread_mutex_unlock(&fileMutex);
        }
    } else if (strcmp(operation, "assign") == 0) {
        char variable[50], valueOrOperation[50];
        sscanf(command, "assign %s %s", variable, valueOrOperation);

        if (strcmp(valueOrOperation, "input") == 0) {
            // Get user input using fgets
            printf("Please enter a value for %s: ", variable);
            char inputValue[1024];
            fgets(inputValue, sizeof(inputValue), stdin);
            
            // Remove the trailing newline character, if present
            inputValue[strcspn(inputValue, "\n")] = '\0';

            setVariableValue(variable, inputValue);
            printf("[DEBUG] Assigned '%s' = '%s' (user input)\n", variable, inputValue);
        } else {
            // Handle constant value assignment
            setVariableValue(variable, valueOrOperation);
            printf("[DEBUG] Assigned '%s' = '%s' (constant value)\n", variable, valueOrOperation);
        }
    } else if (strcmp(operation, "writeFile") == 0) {
        char filenameVariable[50], dataVariable[50];
        if (sscanf(command, "writeFile %s %s", filenameVariable, dataVariable) == 2) {
            writeFile(filenameVariable, dataVariable);
        } else {
            printf("[ERROR] Invalid syntax for writeFile. Expected format: 'writeFile <filenameVariable> <dataVariable>'.\n");
        }
    } else if (strcmp(operation, "printFromTo") == 0) {
        char startVariable[50], endVariable[50];
        if (sscanf(command, "printFromTo %s %s", startVariable, endVariable) == 2) {
            printFromTo(startVariable, endVariable);
        } else {
            printf("[ERROR] Invalid syntax for printFromTo. Expected format: 'printFromTo <start> <end>'.\n");
        }
    } else if (strcmp(operation, "print") == 0) {
        char value[50];
        sscanf(command, "print %s", value);

        char resolvedValue[1024];
        if (getVariableValue(value, resolvedValue)) {
            printf("%s\n", resolvedValue);
        } else {
            printf("%s\n", value); // Print as-is if not a variable
        }
    } else {
        printf("[ERROR] Unknown command: %s\n", command);
    }
}

// Function to execute a program file
void executeProgram(const char *filename) {
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s%s", PROGRAMS_FOLDER, filename);

    FILE *file = fopen(fullPath, "r");
    if (file == NULL) {
        printf("[ERROR] Could not open file. Path: %s\n", fullPath);
        perror("Reason");
        exit(EXIT_FAILURE);
    }

    char command[256];
    while (fgets(command, sizeof(command), file)) {
        command[strcspn(command, "\n")] = 0; // Remove newline
        printf("[INFO] Executing: %s\n", command);
        executeCommand(command);
    }

    fclose(file);
}

int main() {
    char filename[256];
    printf("Enter the filename of the program to execute (from '../../programs/' folder): ");
    scanf("%s", filename);
    getchar(); // Consume the newline character left by scanf

    executeProgram(filename);

    return 0;
}