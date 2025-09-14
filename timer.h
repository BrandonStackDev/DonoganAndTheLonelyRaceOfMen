// timer.h
#ifndef TIMER_H
#define TIMER_H

#include "raylib.h"

typedef struct {
    double duration;   // seconds
    double start;      // seconds from GetTime()
    bool   running;
} Timer;

static inline Timer CreateTimer(double dur) { return (Timer) { dur, 0.0, false }; }
static inline void  StartTimer(Timer* t) { t->start = GetTime(); t->running = true; }
static inline bool  HasTimerElapsed(Timer* t)
{
    double time = GetTime();
    bool res = t->running && (time - t->start) >= t->duration;
    return res;
}
static inline void  ResetTimer(Timer* t) { t->running = false; }

#endif //TIMER_H
