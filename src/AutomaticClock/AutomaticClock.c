#include "AutomaticClock.h"
#include <stdlib.h>
#include <math.h>

#define TRUE 0
#define FALSE 1

AutomaticClock* AutomaticClock_create(double frequency_hz) {
    AutomaticClock* clock = (AutomaticClock*)malloc(sizeof(AutomaticClock));
    if (clock) {
        clock->current_cycle = 0;
        clock->running = FALSE;  // Start in stopped state
        clock->frequency = frequency_hz > 0 ? frequency_hz : 10.0;  // Default to 10 Hz
        clock->last_tick_time = 0.0;
    }
    return clock;
}

void AutomaticClock_destroy(AutomaticClock* clock) {
    free(clock);
}

int AutomaticClock_update(AutomaticClock* clock, double current_time) {
    if (!clock || clock->running == FALSE) return 0;
    
    double time_per_cycle = 1.0 / clock->frequency;
    double elapsed = current_time - clock->last_tick_time;
    int cycles_to_execute = (int)floor(elapsed / time_per_cycle);
    
    if (cycles_to_execute > 0) {
        clock->current_cycle += cycles_to_execute;
        clock->last_tick_time += cycles_to_execute * time_per_cycle;
    }
    
    return cycles_to_execute;
}

int AutomaticClock_getCycle(const AutomaticClock* clock) {
    return clock ? clock->current_cycle : -1;
}

void AutomaticClock_start(AutomaticClock* clock) {
    if (clock) {
        clock->running = TRUE;
    }
}

void AutomaticClock_stop(AutomaticClock* clock) {
    if (clock) {
        clock->running = FALSE;
    }
}

int AutomaticClock_isRunning(const AutomaticClock* clock) {
    return clock ? clock->running : FALSE;
}

void AutomaticClock_setFrequency(AutomaticClock* clock, double frequency_hz) {
    if (clock && frequency_hz > 0) {
        clock->frequency = frequency_hz;
    }
}