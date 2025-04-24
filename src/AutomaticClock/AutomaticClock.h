#ifndef AUTOMATICCLOCK_H
#define AUTOMATICCLOCK_H

typedef struct {
    int current_cycle;
    double interval;   // Time interval in seconds
    int is_running;    // To track if the clock is running
} AutomaticClock;

AutomaticClock* AutomaticClock_create(double interval);
void AutomaticClock_destroy(AutomaticClock* clock);
void AutomaticClock_update(AutomaticClock* clock);
int AutomaticClock_getCycle(const AutomaticClock* clock);
void AutomaticClock_setInterval(AutomaticClock* clock, double interval);
void AutomaticClock_start(AutomaticClock* clock);

#endif // AUTOMATICCLOCK_H
