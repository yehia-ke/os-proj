#ifndef PROCESS_H
#define PROCESS_H

typedef struct {
    char path[256]; // Assuming a maximum path length of 256 characters
    int arrival_time; // Arrival time in seconds or any other unit
} process;

#endif // PROCESS_H