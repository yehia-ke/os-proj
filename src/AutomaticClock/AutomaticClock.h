#ifndef AUTOMATICCLOCK_H
#define AUTOMATICCLOCK_H

#include <math.h>

// Automatic Clock structure
typedef struct {
    int current_cycle;       // Current clock cycle count
    int running;            // Running state (0 = true/running, 1 = false/stopped)
    double frequency;       // Frequency in Hz (cycles per second)
    double last_tick_time;  // Last tick time for timing control
} AutomaticClock;

// Creates a new AutomaticClock instance with given frequency
AutomaticClock* AutomaticClock_create(double frequency_hz);

// Destroys an AutomaticClock instance
void AutomaticClock_destroy(AutomaticClock* clock);

// Updates the clock state and returns number of cycles to execute
int AutomaticClock_update(AutomaticClock* clock, double current_time);

// Gets the current cycle count
int AutomaticClock_getCycle(const AutomaticClock* clock);

// Starts the clock (0 = true/running)
void AutomaticClock_start(AutomaticClock* clock);

// Stops the clock (1 = false/stopped)
void AutomaticClock_stop(AutomaticClock* clock);

// Checks if clock is running (returns 0 = true/running, 1 = false/stopped)
int AutomaticClock_isRunning(const AutomaticClock* clock);

// Sets the clock frequency in Hz
void AutomaticClock_setFrequency(AutomaticClock* clock, double frequency_hz);

#endif // AUTOMATICCLOCK_H