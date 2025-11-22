/**
 * @file main.c
 * @brief Main Entry Point for LightPCRTOS Embedded System
 *
 * This is the board power-on entry point. It initializes all subsystems
 * and starts the main application loop.
 */

#include <stdbool.h>
#include <stdio.h>

#include "business/control/motor_control.h"
#include "business/sensor/sensor_manager.h"
#include "business/system_init.h"
#include "hal_scheduler.h"
#include "hal_timer.h"

/* Application tasks */
static void SensorReadTask(void* arg);
static void MotorControlTask(void* arg);
static void SystemMonitorTask(void* arg);

/**
 * @brief Main entry point
 *
 * Called on board power-on or reset
 */
int main(void)
{
    int ret;
    TaskHandle sensorTask = NULL;
    TaskHandle motorTask = NULL;
    TaskHandle monitorTask = NULL;

    printf("\n");
    printf("*****************************************\n");
    printf("*                                       *\n");
    printf("*       LightPCRTOS v1.0.0             *\n");
    printf("*       Embedded Control System         *\n");
    printf("*                                       *\n");
    printf("*****************************************\n");
    printf("\n");

    /* Initialize entire system */
    ret = SYSTEM_Init();
    if (ret != HAL_OK) {
        printf("[MAIN] FATAL: System initialization failed!\n");
        return -1;
    }

    /* Create application tasks */
    printf("[MAIN] Creating application tasks...\n");

    /* Sensor reading task - High priority */
    TaskConfig sensorConfig = {.priority = 3,
                                .stackSize = 2048,
                                .name = "SensorRead",
                                .entryPoint = SensorReadTask,
                                .arg = NULL};

    ret = HAL_SCHEDULER_CreateTask(&sensorConfig, &sensorTask);
    if (ret != HAL_OK) {
        printf("[MAIN] ERROR: Failed to create sensor task\n");
        goto cleanup;
    }
    printf("[MAIN]   ✓ Sensor task created (priority: 3)\n");

    /* Motor control task - Medium priority */
    TaskConfig motorConfig = {.priority = 2,
                               .stackSize = 2048,
                               .name = "MotorCtrl",
                               .entryPoint = MotorControlTask,
                               .arg = NULL};

    ret = HAL_SCHEDULER_CreateTask(&motorConfig, &motorTask);
    if (ret != HAL_OK) {
        printf("[MAIN] ERROR: Failed to create motor control task\n");
        goto cleanup;
    }
    printf("[MAIN]   ✓ Motor control task created (priority: 2)\n");

    /* System monitor task - Low priority */
    TaskConfig monitorConfig = {.priority = 1,
                                 .stackSize = 1024,
                                 .name = "SysMonitor",
                                 .entryPoint = SystemMonitorTask,
                                 .arg = NULL};

    ret = HAL_SCHEDULER_CreateTask(&monitorConfig, &monitorTask);
    if (ret != HAL_OK) {
        printf("[MAIN] ERROR: Failed to create monitor task\n");
        goto cleanup;
    }
    printf("[MAIN]   ✓ System monitor task created (priority: 1)\n");

    printf("\n[MAIN] System ready - entering main loop\n\n");

    /* Main loop - scheduler takes over */
    while (1) {
        /* Scheduler handles task execution */
        HAL_SCHEDULER_Yield();

        /* Check for system errors */
        SystemInitStatus status;
        if (SYSTEM_GetInitStatus(&status) == HAL_OK) {
            if (status.lastError != HAL_OK) {
                printf("[MAIN] WARNING: System error detected: %d\n", status.lastError);
            }
        }

        /* Keep system alive - simulate idle task */
        HAL_TIMER_DelayMs(100);
    }

cleanup:
    /* Cleanup on error */
    if (monitorTask)
        HAL_SCHEDULER_DeleteTask(monitorTask);
    if (motorTask)
        HAL_SCHEDULER_DeleteTask(motorTask);
    if (sensorTask)
        HAL_SCHEDULER_DeleteTask(sensorTask);

    SYSTEM_Deinit();

    return -1;
}

/**
 * @brief Sensor reading task
 *
 * Periodically reads sensor data and updates system state
 */
static void SensorReadTask(void* arg)
{
    (void)arg;
    SensorData data;
    uint32_t readCount = 0;

    printf("[SENSOR_TASK] Started\n");

    while (1) {
        /* Read sensor data */
        int ret = SENSOR_MANAGER_Read(&data);
        if (ret == HAL_OK && data.valid) {
            readCount++;

            /* Log every 10th reading */
            if (readCount % 10 == 0) {
                printf("[SENSOR_TASK] Reading #%u: Temp=%.1f°C, Pressure=%.0f Pa, "
                       "Humidity=%.0f%%\n",
                       readCount, data.temperature, data.pressure, data.humidity);
            }

            /* Check for abnormal temperature */
            if (data.temperature > 80.0f) {
                printf("[SENSOR_TASK] WARNING: High temperature detected: %.1f°C\n",
                       data.temperature);
                /* Could trigger motor slowdown or shutdown */
                MOTOR_CONTROL_SetSpeed(50);  /* Reduce to 50% */
            }
        }

        /* Run at 10Hz */
        HAL_TIMER_DelayMs(100);
    }
}

/**
 * @brief Motor control task
 *
 * Controls motor based on system state and sensor feedback
 */
static void MotorControlTask(void* arg)
{
    (void)arg;
    uint32_t speedLevel = 0;
    const uint32_t speeds[] = {0, 25, 50, 75, 100};
    const int numSpeeds = sizeof(speeds) / sizeof(speeds[0]);

    printf("[MOTOR_TASK] Started\n");

    /* Start motor in speed mode */
    MOTOR_CONTROL_SetMode(MOTOR_MODE_SPEED);
    MOTOR_CONTROL_SetSpeed(speeds[1]);  /* Start at 25% */
    MOTOR_CONTROL_Start();

    printf("[MOTOR_TASK] Motor started at 25%% speed\n");

    while (1) {
        /* Cycle through speed levels every 5 seconds */
        HAL_TIMER_DelayMs(5000);

        speedLevel = (speedLevel + 1) % numSpeeds;
        uint32_t newSpeed = speeds[speedLevel];

        MOTOR_CONTROL_SetSpeed(newSpeed);
        printf("[MOTOR_TASK] Speed changed to %u%%\n", newSpeed);

        /* Demonstrate mode switching */
        if (speedLevel == 0) {
            MOTOR_CONTROL_Stop();
            printf("[MOTOR_TASK] Motor stopped\n");
        } else if (speedLevel == 1) {
            MOTOR_CONTROL_Start();
            printf("[MOTOR_TASK] Motor restarted\n");
        }
    }
}

/**
 * @brief System monitoring task
 *
 * Monitors system health and reports statistics
 */
static void SystemMonitorTask(void* arg)
{
    (void)arg;
    uint32_t uptimeSeconds = 0;

    printf("[MONITOR_TASK] Started\n");

    while (1) {
        /* Report every 10 seconds */
        HAL_TIMER_DelayMs(10000);
        uptimeSeconds += 10;

        /* Get system status */
        SystemInitStatus status;
        if (SYSTEM_GetInitStatus(&status) == HAL_OK) {
            printf("\n[MONITOR] === System Status (Uptime: %u s) ===\n", uptimeSeconds);
            printf("[MONITOR] Init Stage: %d\n", status.currentStage);
            printf("[MONITOR] HAL Initialized: %s\n", status.halInitialized ? "Yes" : "No");
            printf("[MONITOR] Sensor Initialized: %s\n",
                   status.sensorInitialized ? "Yes" : "No");
            printf("[MONITOR] Motor Initialized: %s\n",
                   status.motorInitialized ? "Yes" : "No");
            printf("[MONITOR] Init Time: %u ms\n", status.initTimeMs);
            printf("[MONITOR] System Tick: %llu ms\n", HAL_TIMER_GetSystemTickMs());
            printf("[MONITOR] =====================================\n\n");
        }

        /* Get sensor sample count */
        uint32_t sampleCount = SENSOR_MANAGER_GetSampleCount();
        printf("[MONITOR] Total sensor samples: %u\n", sampleCount);
    }
}
