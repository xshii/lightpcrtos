/**
 * @file sim_timer.c
 * @brief Timer Simulator Implementation
 */

#include "sim_timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hal_timer.h"

#define MAX_TIMERS 32

/* Timer state */
typedef struct {
    bool allocated;
    bool running;
    TimerId timerId;
    TimerMode mode;
    uint32_t periodUs;
    uint32_t counterUs;
    uint64_t lastTickUs;
    TimerCallback callback;
    void* userData;
} SimTimerState;

/* Simulator state */
static struct {
    bool initialized;
    uint64_t currentTimeUs;
    SimTimerState timers[MAX_TIMERS];
    uint32_t totalTimersCreated;
    uint32_t totalCallbacksFired;
} g_simTimer = {0};

/* Forward declarations */
static void SimTimerProcessCallbacks(uint64_t oldTime, uint64_t newTime);

/* ============================================
 * Simulator Control APIs (SIM_TIMER_*)
 * ============================================ */

int SIM_TIMER_Init(void)
{
    printf("[SIM_TIMER] Simulator initialized\n");
    memset(&g_simTimer, 0, sizeof(g_simTimer));
    g_simTimer.initialized = true;
    return 0;
}

int SIM_TIMER_Reset(void)
{
    printf("[SIM_TIMER] Simulator reset\n");
    memset(&g_simTimer, 0, sizeof(g_simTimer));
    g_simTimer.initialized = true;
    return 0;
}

int SIM_TIMER_AdvanceTime(uint64_t microseconds)
{
    if (!g_simTimer.initialized) {
        return -1;
    }

    uint64_t oldTime = g_simTimer.currentTimeUs;
    g_simTimer.currentTimeUs += microseconds;

    SimTimerProcessCallbacks(oldTime, g_simTimer.currentTimeUs);

    return 0;
}

uint64_t SIM_TIMER_GetCurrentTime(void)
{
    return g_simTimer.currentTimeUs;
}

int SIM_TIMER_SetTime(uint64_t microseconds)
{
    if (!g_simTimer.initialized) {
        return -1;
    }

    uint64_t oldTime = g_simTimer.currentTimeUs;
    g_simTimer.currentTimeUs = microseconds;

    SimTimerProcessCallbacks(oldTime, microseconds);

    return 0;
}

uint32_t SIM_TIMER_GetActiveTimerCount(void)
{
    uint32_t count = 0;
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (g_simTimer.timers[i].allocated && g_simTimer.timers[i].running) {
            count++;
        }
    }
    return count;
}

int SIM_TIMER_GetStats(uint32_t* totalTimers, uint32_t* activeTimers, uint32_t* totalCallbacks)
{
    if (!g_simTimer.initialized) {
        return -1;
    }

    if (totalTimers) {
        *totalTimers = g_simTimer.totalTimersCreated;
    }
    if (activeTimers) {
        *activeTimers = SIM_TIMER_GetActiveTimerCount();
    }
    if (totalCallbacks) {
        *totalCallbacks = g_simTimer.totalCallbacksFired;
    }

    return 0;
}

/* ============================================
 * HAL Timer Implementation (HAL_TIMER_*)
 * ============================================ */

int HAL_TIMER_Init(void)
{
    if (!g_simTimer.initialized) {
        SIM_TIMER_Init();
    }
    return HAL_OK;
}

int HAL_TIMER_Deinit(void)
{
    SIM_TIMER_Reset();
    return HAL_OK;
}

int HAL_TIMER_Create(TimerId timerId, const TimerConfig* config, TimerHandle* handle)
{
    if (!g_simTimer.initialized || !config || !handle) {
        return HAL_ERROR;
    }

    /* Find free slot */
    int slot = -1;
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (!g_simTimer.timers[i].allocated) {
            slot = i;
            break;
        }
    }

    if (slot < 0) {
        printf("[SIM_TIMER] ERROR: No free timer slots\n");
        return HAL_ERROR;
    }

    /* Initialize timer */
    SimTimerState* timer = &g_simTimer.timers[slot];
    memset(timer, 0, sizeof(SimTimerState));
    timer->allocated = true;
    timer->running = false;
    timer->timerId = timerId;
    timer->mode = config->mode;
    timer->periodUs = config->periodUs;
    timer->callback = config->callback;
    timer->userData = config->userData;

    *handle = timer;
    g_simTimer.totalTimersCreated++;

    printf("[SIM_TIMER] Created timer 0x%lx (ID=%u, period=%uus, mode=%s)\n", (unsigned long) timer,
           timerId, config->periodUs, config->mode == TIMER_MODE_ONESHOT ? "oneshot" : "periodic");

    return HAL_OK;
}

int HAL_TIMER_Destroy(TimerHandle handle)
{
    if (!handle) {
        return HAL_ERROR;
    }

    SimTimerState* timer = (SimTimerState*) handle;
    if (!timer->allocated) {
        return HAL_ERROR;
    }

    printf("[SIM_TIMER] Destroyed timer 0x%lx\n", (unsigned long) handle);
    memset(timer, 0, sizeof(SimTimerState));

    return HAL_OK;
}

int HAL_TIMER_Start(TimerHandle handle)
{
    if (!handle) {
        return HAL_ERROR;
    }

    SimTimerState* timer = (SimTimerState*) handle;
    if (!timer->allocated) {
        return HAL_ERROR;
    }

    timer->running = true;
    timer->counterUs = 0;
    timer->lastTickUs = g_simTimer.currentTimeUs;

    printf("[SIM_TIMER] Started timer 0x%lx\n", (unsigned long) handle);

    return HAL_OK;
}

int HAL_TIMER_Stop(TimerHandle handle)
{
    if (!handle) {
        return HAL_ERROR;
    }

    SimTimerState* timer = (SimTimerState*) handle;
    if (!timer->allocated) {
        return HAL_ERROR;
    }

    timer->running = false;

    printf("[SIM_TIMER] Stopped timer 0x%lx\n", (unsigned long) handle);

    return HAL_OK;
}

int HAL_TIMER_Reset(TimerHandle handle)
{
    if (!handle) {
        return HAL_ERROR;
    }

    SimTimerState* timer = (SimTimerState*) handle;
    if (!timer->allocated) {
        return HAL_ERROR;
    }

    timer->counterUs = 0;
    timer->lastTickUs = g_simTimer.currentTimeUs;

    return HAL_OK;
}

int HAL_TIMER_GetCounter(TimerHandle handle, uint32_t* value)
{
    if (!handle || !value) {
        return HAL_ERROR;
    }

    SimTimerState* timer = (SimTimerState*) handle;
    if (!timer->allocated) {
        return HAL_ERROR;
    }

    if (timer->running) {
        uint64_t elapsed = g_simTimer.currentTimeUs - timer->lastTickUs;
        *value = (uint32_t) (timer->counterUs + elapsed);
    } else {
        *value = timer->counterUs;
    }

    return HAL_OK;
}

int HAL_TIMER_SetPeriod(TimerHandle handle, uint32_t periodUs)
{
    if (!handle) {
        return HAL_ERROR;
    }

    SimTimerState* timer = (SimTimerState*) handle;
    if (!timer->allocated) {
        return HAL_ERROR;
    }

    timer->periodUs = periodUs;

    return HAL_OK;
}

int HAL_TIMER_IsRunning(TimerHandle handle, bool* isRunning)
{
    if (!handle || !isRunning) {
        return HAL_ERROR;
    }

    SimTimerState* timer = (SimTimerState*) handle;
    if (!timer->allocated) {
        return HAL_ERROR;
    }

    *isRunning = timer->running;

    return HAL_OK;
}

uint64_t HAL_TIMER_GetSystemTickUs(void)
{
    return g_simTimer.currentTimeUs;
}

uint64_t HAL_TIMER_GetSystemTickMs(void)
{
    return g_simTimer.currentTimeUs / 1000;
}

int HAL_TIMER_DelayUs(uint32_t delayUs)
{
    /* In simulation, delays are instant */
    SIM_TIMER_AdvanceTime(delayUs);
    return HAL_OK;
}

int HAL_TIMER_DelayMs(uint32_t delayMs)
{
    return HAL_TIMER_DelayUs(delayMs * 1000);
}

/* ============================================
 * Internal Helper Functions
 * ============================================ */

static void SimTimerProcessCallbacks(uint64_t oldTime, uint64_t newTime)
{
    if (newTime <= oldTime) {
        return;
    }

    /* Process all running timers */
    for (int i = 0; i < MAX_TIMERS; i++) {
        SimTimerState* timer = &g_simTimer.timers[i];

        if (!timer->allocated || !timer->running) {
            continue;
        }

        uint64_t elapsed = newTime - timer->lastTickUs;
        timer->counterUs += elapsed;

        /* Check if timer period has elapsed */
        while (timer->counterUs >= timer->periodUs) {
            /* Fire callback */
            if (timer->callback) {
                timer->callback(timer, timer->userData);
                g_simTimer.totalCallbacksFired++;
            }

            timer->counterUs -= timer->periodUs;

            /* Handle one-shot timers */
            if (timer->mode == TIMER_MODE_ONESHOT) {
                timer->running = false;
                timer->counterUs = 0;
                break;
            }
        }

        timer->lastTickUs = newTime;
    }
}
