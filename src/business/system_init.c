/**
 * @file system_init.c
 * @brief System Initialization Implementation
 */

#include "system_init.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "control/motor_control.h"
#include "hal_dma.h"
#include "hal_memory.h"
#include "hal_scheduler.h"
#include "hal_timer.h"
#include "hal_types.h"
#include "sensor/sensor_manager.h"

/* Global system status */
static SystemInitStatus g_systemStatus = {0};
static bool g_systemInitialized = false;

/* Forward declarations */
static int InitHardwareLayer(void);
static int InitBusinessLayer(void);
static void LogInitProgress(const char* component, int result);

/**
 * @brief Initialize the entire system
 */
int SYSTEM_Init(void)
{
    int ret;
    uint64_t startTime = 0;

    printf("\n");
    printf("========================================\n");
    printf("  LightPCRTOS System Initialization    \n");
    printf("========================================\n");
    printf("\n");

    memset(&g_systemStatus, 0, sizeof(g_systemStatus));
    g_systemInitialized = false;

    /* Stage 1: Hardware/HAL Layer */
    printf("[INIT] Stage 1: Hardware Layer Initialization\n");
    g_systemStatus.currentStage = INIT_STAGE_HARDWARE;

    ret = InitHardwareLayer();
    if (ret != HAL_OK) {
        printf("[INIT] ERROR: Hardware layer initialization failed\n");
        g_systemStatus.lastError = ret;
        return HAL_ERROR;
    }

    g_systemStatus.halInitialized = true;
    printf("[INIT] Hardware layer initialized successfully\n\n");

    /* Stage 2: Business Layer */
    printf("[INIT] Stage 2: Business Layer Initialization\n");
    g_systemStatus.currentStage = INIT_STAGE_BUSINESS;

    ret = InitBusinessLayer();
    if (ret != HAL_OK) {
        printf("[INIT] ERROR: Business layer initialization failed\n");
        g_systemStatus.lastError = ret;
        return HAL_ERROR;
    }

    printf("[INIT] Business layer initialized successfully\n\n");

    /* Stage 3: Start Scheduler */
    printf("[INIT] Stage 3: Starting System Scheduler\n");

    ret = HAL_SCHEDULER_Start();
    if (ret != HAL_OK) {
        printf("[INIT] ERROR: Failed to start scheduler\n");
        g_systemStatus.lastError = ret;
        return HAL_ERROR;
    }

    LogInitProgress("Scheduler", ret);

    /* Initialization complete */
    g_systemStatus.currentStage = INIT_STAGE_COMPLETE;
    g_systemStatus.initTimeMs = (uint32_t)(HAL_TIMER_GetSystemTickMs() - startTime);
    g_systemInitialized = true;

    printf("\n");
    printf("========================================\n");
    printf("  System Initialization Complete!      \n");
    printf("  Time: %u ms                          \n", g_systemStatus.initTimeMs);
    printf("========================================\n");
    printf("\n");

    return HAL_OK;
}

/**
 * @brief Initialize hardware/HAL layer
 */
static int InitHardwareLayer(void)
{
    int ret;

    /* Initialize Memory subsystem */
    ret = HAL_MEMORY_Init();
    LogInitProgress("Memory", ret);
    if (ret != HAL_OK)
        return ret;

    /* Configure memory pools for the system */
#ifdef HARDWARE_SIMULATION
    /* In simulation, configure pools */
    extern int SIM_MEMORY_ConfigurePool(const char*, void*, size_t);
    SIM_MEMORY_ConfigurePool(POOL_NAME_SRAM, (void*)0x20000000, 1024 * 1024);   /* 1MB SRAM */
    SIM_MEMORY_ConfigurePool(POOL_NAME_L2, (void*)0x10000000, 512 * 1024);      /* 512KB L2 */
    SIM_MEMORY_ConfigurePool(POOL_NAME_L1, (void*)0x08000000, 256 * 1024);      /* 256KB L1 */
    SIM_MEMORY_ConfigurePool(POOL_NAME_DDR, (void*)0x80000000, 64 * 1024 * 1024); /* 64MB DDR */
#endif

    /* Initialize DMA subsystem */
    ret = HAL_DMA_Init(0, NULL);  /* DMA instance 0 with default config */
    LogInitProgress("DMA", ret);
    if (ret != HAL_OK)
        return ret;

    /* Initialize Timer subsystem */
    ret = HAL_TIMER_Init();
    LogInitProgress("Timer", ret);
    if (ret != HAL_OK)
        return ret;

    /* Initialize Scheduler */
    ret = HAL_SCHEDULER_Init();
    LogInitProgress("Scheduler", ret);
    if (ret != HAL_OK)
        return ret;

    return HAL_OK;
}

/**
 * @brief Initialize business layer components
 */
static int InitBusinessLayer(void)
{
    int ret;

    /* Initialize Sensor Manager */
    ret = SENSOR_MANAGER_Init();
    LogInitProgress("Sensor Manager", ret);
    if (ret != HAL_OK) {
        return ret;
    }
    g_systemStatus.sensorInitialized = true;

    /* Initialize Motor Control */
    ret = MOTOR_CONTROL_Init(0);  /* Use DMA instance 0 */
    LogInitProgress("Motor Control", ret);
    if (ret != HAL_OK) {
        return ret;
    }
    g_systemStatus.motorInitialized = true;

    /* Start sensor sampling (100ms period = 10Hz) */
    ret = SENSOR_MANAGER_StartSampling(100);
    LogInitProgress("Sensor Sampling", ret);
    if (ret != HAL_OK) {
        return ret;
    }

    /* Set default motor mode */
    ret = MOTOR_CONTROL_SetMode(MOTOR_MODE_SPEED);
    LogInitProgress("Motor Mode (Speed)", ret);
    if (ret != HAL_OK) {
        return ret;
    }

    return HAL_OK;
}

/**
 * @brief Deinitialize the entire system
 */
int SYSTEM_Deinit(void)
{
    printf("\n[INIT] System shutdown initiated\n");

    /* Stop business components first */
    if (g_systemStatus.motorInitialized) {
        MOTOR_CONTROL_Stop();
        MOTOR_CONTROL_Deinit();
        printf("[INIT] Motor Control deinitialized\n");
    }

    if (g_systemStatus.sensorInitialized) {
        SENSOR_MANAGER_StopSampling();
        SENSOR_MANAGER_Deinit();
        printf("[INIT] Sensor Manager deinitialized\n");
    }

    /* Stop scheduler */
    if (g_systemStatus.halInitialized) {
        HAL_SCHEDULER_Stop();
        printf("[INIT] Scheduler stopped\n");
    }

    /* Deinitialize HAL */
    if (g_systemStatus.halInitialized) {
        HAL_TIMER_Deinit();
        HAL_DMA_Deinit(0);  /* DMA instance 0 */
        HAL_MEMORY_Deinit();
        printf("[INIT] HAL layer deinitialized\n");
    }

    g_systemInitialized = false;
    memset(&g_systemStatus, 0, sizeof(g_systemStatus));

    printf("[INIT] System shutdown complete\n\n");

    return HAL_OK;
}

/**
 * @brief Get current system initialization status
 */
int SYSTEM_GetInitStatus(SystemInitStatus* status)
{
    if (!status) {
        return HAL_ERROR;
    }

    memcpy(status, &g_systemStatus, sizeof(SystemInitStatus));
    return HAL_OK;
}

/**
 * @brief Check if system is fully initialized
 */
bool SYSTEM_IsInitialized(void)
{
    return g_systemInitialized;
}

/**
 * @brief Log initialization progress
 */
static void LogInitProgress(const char* component, int result)
{
    if (result == HAL_OK) {
        printf("[INIT]   ✓ %s initialized\n", component);
    } else {
        printf("[INIT]   ✗ %s initialization failed (error: %d)\n", component, result);
    }
}
