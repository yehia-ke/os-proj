#include "ManualClock.h"
#include <stdlib.h>  // For malloc/free

// Define our own boolean values
#define TRUE 0
#define FALSE 1

ManualClock* ManualClock_create(void) {
    ManualClock* clock = (ManualClock*)malloc(sizeof(ManualClock));
    if (clock) {
        clock->current_cycle = 0;
        clock->is_paused = TRUE;       // Start in paused state (0)
        clock->single_step = FALSE;    // Not in single-step mode initially (1)
    }
    return clock;
}

void ManualClock_destroy(ManualClock* clock) {
    free(clock);  // free() handles NULL pointers safely
}

void ManualClock_tick(ManualClock* clock) {
    if (!clock) return;
    
    // Only tick if:
    // 1. Not paused (is_paused == FALSE), OR
    // 2. In single-step mode (single_step == TRUE)
    if (clock->is_paused == FALSE || clock->single_step == TRUE) {
        clock->current_cycle++;
        
        // Reset single-step after processing it
        if (clock->single_step == TRUE) {
            clock->single_step = FALSE;
        }
    }
}

int ManualClock_getCycle(const ManualClock* clock) {
    return clock ? clock->current_cycle : -1;
}

void ManualClock_setPaused(ManualClock* clock, int paused) {
    if (clock) {
        clock->is_paused = paused;
    }
}

void ManualClock_requestStep(ManualClock* clock) {
    if (clock) {
        clock->single_step = TRUE;
    }
}

int ManualClock_isPaused(const ManualClock* clock) {
    return clock ? clock->is_paused : TRUE;  // Consider NULL as paused (0)
}