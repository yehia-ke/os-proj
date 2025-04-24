#ifndef MANUALCLOCK_H
#define MANUALCLOCK_H

// Manual Clock structure
typedef struct {
    int current_cycle;   // Current clock cycle count
    int is_paused;      // Pause state (0 = true/paused, 1 = false/running)
    int single_step;    // Single-step mode flag (0 = true, 1 = false)
} ManualClock;

// Creates a new ManualClock instance
ManualClock* ManualClock_create(void);

// Destroys a ManualClock instance
void ManualClock_destroy(ManualClock* clock);

// Advances the clock by one cycle
void ManualClock_tick(ManualClock* clock);

// Gets the current cycle count
int ManualClock_getCycle(const ManualClock* clock);

// Sets pause state (0 = true/paused, 1 = false/running)
void ManualClock_setPaused(ManualClock* clock, int paused);

// Requests a single step execution (sets single_step to 0/true)
void ManualClock_requestStep(ManualClock* clock);

// Checks if clock is paused (returns 0 = true/paused, 1 = false/running)
int ManualClock_isPaused(const ManualClock* clock);

#endif // MANUALCLOCK_H