/**
 * @file sim_timer.h
 * @brief Timer Simulator for Testing
 */

#ifndef SIM_TIMER_H
#define SIM_TIMER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialize timer simulator
 * @return 0 on success, -1 on failure
 */
int SIM_TIMER_Init(void);

/**
 * @brief Reset timer simulator to initial state
 * @return 0 on success, -1 on failure
 */
int SIM_TIMER_Reset(void);

/**
 * @brief Advance simulated time
 * @param microseconds Time to advance in microseconds
 * @return 0 on success, -1 on failure
 * @note This will trigger any timer callbacks that should fire
 */
int SIM_TIMER_AdvanceTime(uint64_t microseconds);

/**
 * @brief Get current simulated time
 * @return Current simulated time in microseconds
 */
uint64_t SIM_TIMER_GetCurrentTime(void);

/**
 * @brief Set simulated time directly
 * @param microseconds Absolute time in microseconds
 * @return 0 on success, -1 on failure
 */
int SIM_TIMER_SetTime(uint64_t microseconds);

/**
 * @brief Get number of active timers
 * @return Number of active timers
 */
uint32_t SIM_TIMER_GetActiveTimerCount(void);

/**
 * @brief Get simulator statistics
 * @param totalTimers Output total timers created
 * @param activeTimers Output currently active timers
 * @param totalCallbacks Output total callbacks fired
 * @return 0 on success, -1 on failure
 */
int SIM_TIMER_GetStats(uint32_t* totalTimers, uint32_t* activeTimers,
                       uint32_t* totalCallbacks);

#endif /* SIM_TIMER_H */
