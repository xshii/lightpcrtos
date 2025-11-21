/**
 * @file framework_timer.c
 * @brief Business runtime timer framework
 */

#include "framework_timer.h"
#include <string.h>

#define MAX_TIMERS 32

typedef struct {
    FrameworkTimerCallback callback;
    uint32_t periodMs;
    uint32_t lastTriggerMs;
    void* userData;
    int isActive;
} TimerEntry;

static struct {
    uint64_t currentTimeMs;
    TimerEntry timers[MAX_TIMERS];
    int timerCount;
} g_frameworkTimer = {0};

int FRAMEWORK_TIMER_Init(void)
{
    memset(&g_frameworkTimer, 0, sizeof(g_frameworkTimer));
    return 0;
}

int FRAMEWORK_TIMER_AdvanceMs(uint32_t ms)
{
    g_frameworkTimer.currentTimeMs += ms;

    for (int i = 0; i < g_frameworkTimer.timerCount; i++) {
        TimerEntry* timer = &g_frameworkTimer.timers[i];

        if (!timer->isActive || !timer->callback) {
            continue;
        }

        uint32_t elapsed = (uint32_t) (g_frameworkTimer.currentTimeMs - timer->lastTriggerMs);

        if (elapsed >= timer->periodMs) {
            timer->callback(timer->userData);
            timer->lastTriggerMs = (uint32_t) g_frameworkTimer.currentTimeMs;
        }
    }

    return 0;
}

uint64_t FRAMEWORK_TIMER_GetTimeMs(void)
{
    return g_frameworkTimer.currentTimeMs;
}

int FRAMEWORK_TIMER_RegisterCallback(FrameworkTimerCallback callback, uint32_t periodMs,
                                     void* userData)
{
    if (!callback || periodMs == 0) {
        return -1;
    }

    if (g_frameworkTimer.timerCount >= MAX_TIMERS) {
        return -1;
    }

    int timerId = g_frameworkTimer.timerCount++;
    TimerEntry* timer = &g_frameworkTimer.timers[timerId];

    timer->callback = callback;
    timer->periodMs = periodMs;
    timer->lastTriggerMs = (uint32_t) g_frameworkTimer.currentTimeMs;
    timer->userData = userData;
    timer->isActive = 1;

    return timerId;
}

int FRAMEWORK_TIMER_UnregisterCallback(int timerId)
{
    if (timerId < 0 || timerId >= g_frameworkTimer.timerCount) {
        return -1;
    }

    TimerEntry* timer = &g_frameworkTimer.timers[timerId];
    timer->isActive = 0;
    timer->callback = NULL;

    return 0;
}

int FRAMEWORK_TIMER_Reset(void)
{
    g_frameworkTimer.currentTimeMs = 0;

    for (int i = 0; i < g_frameworkTimer.timerCount; i++) {
        g_frameworkTimer.timers[i].lastTriggerMs = 0;
    }

    return 0;
}
