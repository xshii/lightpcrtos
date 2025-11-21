/**
 * @file hal_timer.h
 * @brief Hardware Timer Abstraction Layer
 */

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include "hal_types.h"

/* Timer ID type */
typedef uint32_t TimerId;

/* Timer handle (opaque pointer) */
typedef void* TimerHandle;

/* Timer mode */
typedef enum { TIMER_MODE_ONESHOT = 0, TIMER_MODE_PERIODIC = 1 } TimerMode;

/* Timer callback function */
typedef void (*TimerCallback)(TimerHandle handle, void* userData);

/* Timer configuration */
typedef struct {
    TimerMode mode;
    uint32_t periodUs; /* Period in microseconds */
    TimerCallback callback;
    void* userData;
    uint8_t priority; /* Interrupt priority (0-15) */
} TimerConfig;

/**
 * @brief Initialize timer subsystem
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_TIMER_Init(void);

/**
 * @brief Deinitialize timer subsystem
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_TIMER_Deinit(void);

/**
 * @brief Create a timer
 * @param timerId Hardware timer ID
 * @param config Timer configuration
 * @param handle Output timer handle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_TIMER_Create(TimerId timerId, const TimerConfig* config, TimerHandle* handle);

/**
 * @brief Destroy a timer
 * @param handle Timer handle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_TIMER_Destroy(TimerHandle handle);

/**
 * @brief Start a timer
 * @param handle Timer handle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_TIMER_Start(TimerHandle handle);

/**
 * @brief Stop a timer
 * @param handle Timer handle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_TIMER_Stop(TimerHandle handle);

/**
 * @brief Reset timer counter
 * @param handle Timer handle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_TIMER_Reset(TimerHandle handle);

/**
 * @brief Get current timer counter value
 * @param handle Timer handle
 * @param value Output counter value in microseconds
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_TIMER_GetCounter(TimerHandle handle, uint32_t* value);

/**
 * @brief Set timer period
 * @param handle Timer handle
 * @param periodUs New period in microseconds
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_TIMER_SetPeriod(TimerHandle handle, uint32_t periodUs);

/**
 * @brief Check if timer is running
 * @param handle Timer handle
 * @param isRunning Output running state
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_TIMER_IsRunning(TimerHandle handle, bool* isRunning);

/**
 * @brief Get system tick count (microseconds since boot)
 * @return Tick count in microseconds
 */
uint64_t HAL_TIMER_GetSystemTickUs(void);

/**
 * @brief Get system tick count (milliseconds since boot)
 * @return Tick count in milliseconds
 */
uint64_t HAL_TIMER_GetSystemTickMs(void);

/**
 * @brief Delay for specified microseconds
 * @param delayUs Delay in microseconds
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_TIMER_DelayUs(uint32_t delayUs);

/**
 * @brief Delay for specified milliseconds
 * @param delayMs Delay in milliseconds
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_TIMER_DelayMs(uint32_t delayMs);

#endif /* HAL_TIMER_H */
