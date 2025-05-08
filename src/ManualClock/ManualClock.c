#include "ManualClock.h"
#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

#define TRUE 1
#define FALSE 0

ManualClock* ManualClock_create() {
    printf("Creating ManualClock instance...\n");
    ManualClock* clock = (ManualClock*)malloc(sizeof(ManualClock));
    if (clock) {
        printf("ManualClock instance created successfully.\n");
        clock->current_cycle = 0;
        clock->is_paused = TRUE;     
    }
    printf("ManualClock instance created.\n");
    return clock;
}

void ManualClock_destroy(ManualClock* clock) {
    free(clock);
}

void ManualClock_tick(ManualClock* clock) {
    if (!clock) return;

    clock->current_cycle++;
}

int ManualClock_getCycle(const ManualClock* clock) {
    return clock ? clock->current_cycle : -1;
}