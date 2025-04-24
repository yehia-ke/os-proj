#include "AutomaticClock.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  // For usleep

#define TRUE 1
#define FALSE 0

// Create a new AutomaticClock with a specified interval in seconds (default 0.5 seconds)
AutomaticClock* AutomaticClock_create(double interval) {
    AutomaticClock* clock = (AutomaticClock*)malloc(sizeof(AutomaticClock));
    if (clock) {
        clock->current_cycle = 0;
        clock->interval = (interval > 0) ? interval : 0.5;  // Default to 0.5 seconds if invalid input
        clock->is_running = FALSE;  // Initially not running
    }
    return clock;
}

// Destroy the AutomaticClock
void AutomaticClock_destroy(AutomaticClock* clock) {
    free(clock);
}

// Start the AutomaticClock (begin the automatic cycle incrementing)
void AutomaticClock_start(AutomaticClock* clock) {
    if (clock) {
        clock->is_running = TRUE;  // Set the clock as running
    }
}

// Update the clock by incrementing the cycle based on the time interval
void AutomaticClock_update(AutomaticClock* clock) {
    if (!clock || !clock->is_running) return;

    // Wait for the specified interval (in seconds)
    usleep(clock->interval * 1000000);  // Convert to microseconds
    clock->current_cycle++;
}

// Get the current cycle
int AutomaticClock_getCycle(const AutomaticClock* clock) {
    return clock ? clock->current_cycle : -1;
}

// Set the time interval at which the clock should increment (in seconds)
void AutomaticClock_setInterval(AutomaticClock* clock, double interval) {
    if (clock && interval > 0) {
        clock->interval = interval;
    }
}
