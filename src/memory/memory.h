#ifndef FCFS_SCHEDULER_H
#define FCFS_SCHEDULER_H

void set_memory_word(int index, char* memorykhaledhassanword);
char* get_memory_word(int index);
void free_memory(int index, int size);
int allocate_memory(int size);
void initialize_mem();



#endif