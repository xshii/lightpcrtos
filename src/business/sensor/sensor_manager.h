/**
 * @file sensor_manager.h
 * @brief Sensor Manager Module
 */

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "hal_types.h"
#include "hal_memory.h"
#include "hal_dma.h"

/* Sensor data structure */
typedef struct {
    float temperature;
    float humidity;
    uint64_t timestamp;
    bool valid;
} SensorData;

/**
 * @brief Initialize sensor manager
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int SENSOR_MANAGER_Init(void);

/**
 * @brief Deinitialize sensor manager
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int SENSOR_MANAGER_Deinit(void);

/**
 * @brief Read sensor data
 * @param data Output sensor data
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int SENSOR_MANAGER_Read(SensorData* data);

/**
 * @brief Start periodic sampling
 * @param periodMs Sampling period in milliseconds
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int SENSOR_MANAGER_StartSampling(uint32_t periodMs);

/**
 * @brief Stop periodic sampling
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int SENSOR_MANAGER_StopSampling(void);

/**
 * @brief Get sample count
 * @return Number of samples collected
 */
uint32_t SENSOR_MANAGER_GetSampleCount(void);

#endif /* SENSOR_MANAGER_H */
