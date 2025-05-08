#include "AutomaticClock.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include "../main.h" // For usleep

#define TRUE 1
#define FALSE 0

AutomaticClock* AutomaticClock_create(double interval) {
    AutomaticClock* clock = (AutomaticClock*)malloc(sizeof(AutomaticClock));
    if (clock) {
        clock->current_cycle = 0;
        clock->interval = (interval > 0) ? interval : 0.5;
        clock->is_running = FALSE; 
    }
    return clock;
}

void AutomaticClock_destroy(AutomaticClock* clock) {
    free(clock);
}

void AutomaticClock_start(AutomaticClock* clock) {
    if (clock) {
        clock->is_running = TRUE;
    }
}

void AutomaticClock_update(AutomaticClock* clock) {
    if (!clock || !clock->is_running) return;
    clock->current_cycle++;
    int clocktemp = AutomaticClock_getCycle(automaticClock);
    char clockcyclemichael[20];
    sprintf(clockcyclemichael, "%d", clocktemp);
    char message[100];
    snprintf(message, sizeof(message), "Current Clock Cycle: %s", clockcyclemichael);
    set_clocklabel(message);
}

int AutomaticClock_getCycle(const AutomaticClock* clock) {
    return clock ? clock->current_cycle : -1;
}

void AutomaticClock_setInterval(AutomaticClock* clock, double interval) {
    if (clock && interval > 0) {
        clock->interval = interval;
    }
}
