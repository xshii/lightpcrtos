/**
 * @file framework_timer.h
 * @brief Framework Timer - Controllable time for testing
 */

#ifndef FRAMEWORK_TIMER_H
#define FRAMEWORK_TIMER_H

#include <stdint.h>
#include <stdbool.h>

/* Timer callback function */
typedef void (*FrameworkTimerCallback)(void* userData);

/**
 * @brief Initialize framework timer
 * @return 0 on success, -1 on failure
 */
int FRAMEWORK_TIMER_Init(void);

/**
 * @brief Advance time by specified milliseconds
 * @param ms Milliseconds to advance
 * @return 0 on success, -1 on failure
 */
int FRAMEWORK_TIMER_AdvanceMs(uint32_t ms);

/**
 * @brief Get current framework time in milliseconds
 * @return Current time in milliseconds
 */
uint64_t FRAMEWORK_TIMER_GetTimeMs(void);

/**
 * @brief Register periodic timer callback
 * @param callback Callback function
 * @param periodMs Period in milliseconds
 * @param userData User data passed to callback
 * @return Timer ID on success, -1 on failure
 */
int FRAMEWORK_TIMER_RegisterCallback(FrameworkTimerCallback callback,
                                     uint32_t periodMs, void* userData);

/**
 * @brief Unregister timer callback
 * @param timerId Timer ID returned from RegisterCallback
 * @return 0 on success, -1 on failure
 */
int FRAMEWORK_TIMER_UnregisterCallback(int timerId);

/**
 * @brief Reset framework timer to zero
 * @return 0 on success, -1 on failure
 */
int FRAMEWORK_TIMER_Reset(void);

#endif /* FRAMEWORK_TIMER_H */
