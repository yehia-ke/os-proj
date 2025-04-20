#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MEMSIZE  60
static char* memory[MEMSIZE];

void initialize_mem(){
    for(int i = 0; i < MEMSIZE; i++){
        memory[i] = NULL;
    }
}

void free_memory(int i, int s){
    if(i > MEMSIZE || i < 0 || s >= MEMSIZE || s < 0 || i > s){
        printf("Error: Input index value outside scope of memory\n");
        return;
    }
    for(int k = i; k <= s; k++){
        memory[k] = NULL;
    }
}

int allocate_memory(int pSize){
    if(pSize <= 0 || pSize > MEMSIZE) {
        return -1;
    }
    for(int k = 0; k < MEMSIZE; k++){
        if(memory[k] == NULL){
            if(k + pSize > MEMSIZE){
                return -1;
            }
            int j;
            for(j = k; j < k + pSize; j++){
                if(memory[j] != NULL){
                    k = j;
                    break;
                }
            }
            if(j == k + pSize){
                for(j = k; j < k + pSize; j++){
                    memory[j] = "Allocated";
                }
                return k;
            }
        }
    }
    return -1;
}

void set_memory_word(int h, char* input){
    if(h >= MEMSIZE || h < 0){
        printf("Error: Input index value outside scope of memory\n");
        return;
    }
    memory[h] = strdup(input);
}

char* get_memory_word(int index){
    if(index >= MEMSIZE || index < 0){
        printf("Error: Input index value outside scope of memory\n");
        return NULL;
    }
    return memory[index];
}