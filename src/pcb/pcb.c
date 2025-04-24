#include "pcb.h"
#include <stdio.h>
#include <string.h>

PCB* create_process(int pid, char* program_file, int arrival_time) {
    printf("ya");
    char file_path[256];
    sprintf(file_path, "../programs/%s", program_file);
    FILE* fp = fopen(file_path, "r");
    if (!fp) {
        printf("%s\n", file_path);
        printf("Error: Could not open program file %s\n", program_file);
        return NULL;
    }
    printf("1");
    int num_instructions = 0;
    char line[100];
    while (fgets(line, sizeof(line), fp)) {
        num_instructions++;
    }
    fclose(fp);
    printf("2");
    int total_words = 6 + num_instructions + 3;  // PCB + instructions + variables
    int mem_lower = allocate_memory(total_words);
    if (mem_lower == -1) {
        printf("Error: Failed to allocate memory for process %d\n", pid);
        return NULL;
    }
    int mem_upper = mem_lower + total_words - 1;
    printf("3");
    char temp[200];

    sprintf(temp, "PCB_PID:%d", pid);
    set_memory_word(mem_lower, temp);

    sprintf(temp, "PCB_State:%s", "Ready");
    set_memory_word(mem_lower + 1, temp);
    
    sprintf(temp, "PCB_Priority:%d", 1);
    set_memory_word(mem_lower + 2, temp);

    sprintf(temp, "PCB_PC:%d", 0);
    set_memory_word(mem_lower + 3, temp);
    
    sprintf(temp, "PCB_MemLower:%d", mem_lower);
    set_memory_word(mem_lower + 4, temp);
    
    sprintf(temp, "PCB_MemUpper:%d", mem_upper);
    set_memory_word(mem_lower + 5, temp);
    
    printf("4");
    fp = fopen(file_path, "r");
    for (int i = 0; i < num_instructions; i++) {
        fgets(line, sizeof(line), fp);
        line[strcspn(line, "\n")] = 0;
        sprintf(temp, "Instr%d:%s", i, line);
        set_memory_word(mem_lower + 9 + i, temp);
    }
    fclose(fp);
    printf("5");
    set_memory_word(mem_lower + 6, "var1:");
    set_memory_word(mem_lower + 7, "var2:");
    set_memory_word(mem_lower + 8, "var3:");
    printf("6");
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    if (!pcb) {
        printf("Error: Failed to allocate PCB struct\n");
        return NULL;
    }
    pcb->pid = pid;
    pcb->state = strdup("Ready");
    pcb->priority = 1;
    pcb->pc = 0;
    pcb->mem_lower = mem_lower;
    pcb->mem_upper = mem_upper;
    pcb->arrival_time = arrival_time;
    printf("7");
    printf("Process %d created at memory [%d, %d]\n", pid, mem_lower, mem_upper);
    return pcb;
}

void set_state(PCB* pcb, char* new_state) {
    free(pcb->state);
    pcb->state = strdup(new_state);
    char temp[50];
    sprintf(temp, "PCB_State:%s", new_state);
    set_memory_word(pcb->mem_lower + 1, temp);
}

void set_priority(PCB* pcb, int new_priority) {
    pcb->priority = new_priority;
    char temp[50];
    sprintf(temp, "PCB_Priority:%d", new_priority);
    set_memory_word(pcb->mem_lower + 2, temp);
}

char* get_instruction(PCB* pcb) {
    int instr_index = pcb->mem_lower + 9 + pcb->pc;
    if (instr_index > pcb->mem_upper) return NULL;
    char* word = get_memory_word(instr_index);
    if (!word) return NULL;
    char* colon = strchr(word, ':');
    return colon ? colon + 1 : NULL;
}

void set_variable(PCB* pcb, char* var_name, char* value) {
    int var_offset = pcb->mem_lower + 6;
    int index = var_offset + (strcmp(var_name, "var1") == 0 ? 0 :
                             strcmp(var_name, "var2") == 0 ? 1 : 2);
    char temp[50];
    sprintf(temp, "%s:%s", var_name, value);
    set_memory_word(index, temp);
}

void free_process(PCB* pcb) {
    free_memory(pcb->mem_lower, pcb->mem_upper);
    free(pcb->state);
    free(pcb);
}