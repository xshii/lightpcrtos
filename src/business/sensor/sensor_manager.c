/**
 * @file sensor_manager.c
 * @brief Sensor Manager Implementation
 */

#include "sensor_manager.h"
#include "hal_scheduler.h"
#include <string.h>

/* Module state */
static struct {
    bool initialized;
    bool sampling;
    uint32_t sampleCount;
    uint32_t periodMs;
    MemoryBuffer dataBuffer;
} g_sensorManager = {0};

/* Private functions (模块内对内接口: 大驼峰) */
static int SensorManagerAllocateDataBuffer(void) {
    return HAL_MEMORY_AllocBuffer(POOL_NAME_SRAM, sizeof(SensorData) * 100,
                                  &g_sensorManager.dataBuffer);
}

static void SensorManagerFreeDataBuffer(void) {
    if (g_sensorManager.dataBuffer) {
        HAL_MEMORY_FreeBuffer(g_sensorManager.dataBuffer);
        g_sensorManager.dataBuffer = NULL;
    }
}

/* Public functions (对外接口: 全大写+下划线) */
int SENSOR_MANAGER_Init(void) {
    if (g_sensorManager.initialized) {
        return HAL_OK;
    }

    /* Allocate data buffer */
    if (SensorManagerAllocateDataBuffer() != HAL_OK) {
        return HAL_ERROR;
    }

    g_sensorManager.initialized = true;
    g_sensorManager.sampling = false;
    g_sensorManager.sampleCount = 0;

    return HAL_OK;
}

int SENSOR_MANAGER_Deinit(void) {
    if (!g_sensorManager.initialized) {
        return HAL_OK;
    }

    /* Free data buffer */
    SensorManagerFreeDataBuffer();

    /* Reset state */
    memset(&g_sensorManager, 0, sizeof(g_sensorManager));

    return HAL_OK;
}

int SENSOR_MANAGER_Read(SensorData* data) {
    if (!g_sensorManager.initialized || !data) {
        return HAL_ERROR;
    }

    /* TODO: Read from actual sensor hardware via HAL */
    /* For now, return dummy data */
    data->temperature = 25.0f;
    data->humidity = 50.0f;
    data->timestamp = 0;
    data->valid = true;

    g_sensorManager.sampleCount++;

    return HAL_OK;
}

int SENSOR_MANAGER_StartSampling(uint32_t periodMs) {
    if (!g_sensorManager.initialized) {
        return HAL_ERROR;
    }

    g_sensorManager.sampling = true;
    g_sensorManager.periodMs = periodMs;

    /* TODO: Register with scheduler for periodic sampling */

    return HAL_OK;
}

int SENSOR_MANAGER_StopSampling(void) {
    g_sensorManager.sampling = false;
    return HAL_OK;
}

uint32_t SENSOR_MANAGER_GetSampleCount(void) {
    return g_sensorManager.sampleCount;
}
