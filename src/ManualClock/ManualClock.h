#ifndef MANUALCLOCK_H
#define MANUALCLOCK_H

typedef struct {
    int current_cycle;   
    int is_paused;      
} ManualClock;

ManualClock* ManualClock_create(void);

void ManualClock_destroy(ManualClock* clock);

void ManualClock_tick(ManualClock* clock);

int ManualClock_getCycle(const ManualClock* clock);

#endif