#ifndef AUTOMATICCLOCK_H
#define AUTOMATICCLOCK_H

typedef struct {
    int current_cycle;
    double interval;  
    int is_running; 
} AutomaticClock;

AutomaticClock* AutomaticClock_create(double interval);
void AutomaticClock_destroy(AutomaticClock* clock);
void AutomaticClock_update(AutomaticClock* clock);
int AutomaticClock_getCycle(const AutomaticClock* clock);
void AutomaticClock_setInterval(AutomaticClock* clock, double interval);
void AutomaticClock_start(AutomaticClock* clock);

#endif 
