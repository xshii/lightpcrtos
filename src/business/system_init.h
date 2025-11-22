/**
 * @file system_init.h
 * @brief System Initialization Module
 *
 * This module handles board power-on initialization sequence for all
 * business components in the correct order.
 */

#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief System initialization stages
 */
typedef enum {
    INIT_STAGE_HARDWARE = 0,  /**< Hardware/HAL initialization */
    INIT_STAGE_DRIVERS,       /**< Driver initialization */
    INIT_STAGE_BUSINESS,      /**< Business logic initialization */
    INIT_STAGE_COMPLETE       /**< Initialization complete */
} InitStage;

/**
 * @brief System initialization status
 */
typedef struct {
    InitStage currentStage;
    bool halInitialized;
    bool sensorInitialized;
    bool motorInitialized;
    uint32_t initTimeMs;
    int lastError;
} SystemInitStatus;

/**
 * @brief Initialize the entire system
 *
 * Performs complete board power-on initialization sequence:
 * 1. HAL layer (Memory, DMA, Timer, Scheduler)
 * 2. Business components (Sensor Manager, Motor Control)
 * 3. Start system scheduler
 *
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int SYSTEM_Init(void);

/**
 * @brief Deinitialize the entire system
 *
 * Cleanly shuts down all components in reverse order
 *
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int SYSTEM_Deinit(void);

/**
 * @brief Get current system initialization status
 *
 * @param[out] status Pointer to status structure to fill
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int SYSTEM_GetInitStatus(SystemInitStatus* status);

/**
 * @brief Check if system is fully initialized
 *
 * @return true if fully initialized, false otherwise
 */
bool SYSTEM_IsInitialized(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_INIT_H */
