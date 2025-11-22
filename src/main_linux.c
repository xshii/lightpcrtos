/**
 * @file main_linux.c
 * @brief Main Entry Point for LightPCRTOS on Linux Platform
 *
 * Linux-specific implementation with POSIX threads for task simulation
 */

#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "business/control/motor_control.h"
#include "business/sensor/sensor_manager.h"
#include "business/system_init.h"
#include "hal_scheduler.h"
#include "hal_timer.h"

/* Global flag for graceful shutdown */
static volatile bool g_running = true;

/* Signal handler for Ctrl+C */
static void signal_handler(int signum)
{
    (void)signum;
    printf("\n[MAIN] Received shutdown signal...\n");
    g_running = false;
}

/* Application task functions */
static void* SensorReadTask(void* arg);
static void* MotorControlTask(void* arg);
static void* SystemMonitorTask(void* arg);

/**
 * @brief Main entry point for Linux platform
 */
int main(void)
{
    int ret;
    pthread_t sensorThread, motorThread, monitorThread;

    printf("\n");
    printf("*****************************************\n");
    printf("*                                       *\n");
    printf("*       LightPCRTOS v1.0.0             *\n");
    printf("*       Linux Platform Edition          *\n");
    printf("*                                       *\n");
    printf("*****************************************\n");
    printf("\n");

    /* Register signal handlers for graceful shutdown */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* Initialize entire system */
    ret = SYSTEM_Init();
    if (ret != HAL_OK) {
        printf("[MAIN] FATAL: System initialization failed!\n");
        return EXIT_FAILURE;
    }

    /* Create application threads (simulate RTOS tasks) */
    printf("[MAIN] Creating application threads...\n");

    ret = pthread_create(&sensorThread, NULL, SensorReadTask, NULL);
    if (ret != 0) {
        printf("[MAIN] ERROR: Failed to create sensor thread\n");
        goto cleanup;
    }
    printf("[MAIN]   ✓ Sensor thread created\n");

    ret = pthread_create(&motorThread, NULL, MotorControlTask, NULL);
    if (ret != 0) {
        printf("[MAIN] ERROR: Failed to create motor thread\n");
        goto cleanup;
    }
    printf("[MAIN]   ✓ Motor thread created\n");

    ret = pthread_create(&monitorThread, NULL, SystemMonitorTask, NULL);
    if (ret != 0) {
        printf("[MAIN] ERROR: Failed to create monitor thread\n");
        goto cleanup;
    }
    printf("[MAIN]   ✓ Monitor thread created\n");

    printf("\n[MAIN] System ready - Press Ctrl+C to shutdown\n\n");

    /* Main loop - wait for shutdown signal */
    while (g_running) {
        sleep(1);

        /* Check for system errors */
        SystemInitStatus status;
        if (SYSTEM_GetInitStatus(&status) == HAL_OK) {
            if (status.lastError != HAL_OK) {
                printf("[MAIN] WARNING: System error detected: %d\n", status.lastError);
            }
        }
    }

    printf("\n[MAIN] Initiating graceful shutdown...\n");

    /* Wait for threads to finish */
    pthread_join(sensorThread, NULL);
    pthread_join(motorThread, NULL);
    pthread_join(monitorThread, NULL);

    printf("[MAIN] All threads terminated\n");

cleanup:
    /* Cleanup system */
    SYSTEM_Deinit();

    printf("\n[MAIN] Shutdown complete. Goodbye!\n\n");

    return EXIT_SUCCESS;
}

/**
 * @brief Sensor reading thread
 */
static void* SensorReadTask(void* arg)
{
    (void)arg;
    SensorData data;
    uint32_t readCount = 0;

    printf("[SENSOR_THREAD] Started (PID: %d)\n", getpid());

    while (g_running) {
        /* Read sensor data */
        int ret = SENSOR_MANAGER_Read(&data);
        if (ret == HAL_OK && data.valid) {
            readCount++;

            /* Log every 10th reading */
            if (readCount % 10 == 0) {
                printf("[SENSOR_THREAD] Reading #%u: Temp=%.1f°C, Humidity=%.0f%%, "
                       "Timestamp=%llu\n",
                       readCount, data.temperature, data.humidity,
                       (unsigned long long)data.timestamp);
            }

            /* Check for abnormal temperature */
            if (data.temperature > 80.0f) {
                printf("[SENSOR_THREAD] WARNING: High temperature detected: %.1f°C\n",
                       data.temperature);
                /* Could trigger motor slowdown */
                MOTOR_CONTROL_SetSpeed(50);
            }
        }

        /* Run at 10Hz */
        usleep(100000);  /* 100ms */
    }

    printf("[SENSOR_THREAD] Exiting...\n");
    return NULL;
}

/**
 * @brief Motor control thread
 */
static void* MotorControlTask(void* arg)
{
    (void)arg;
    uint32_t speedLevel = 0;
    const uint32_t speeds[] = {0, 25, 50, 75, 100};
    const int numSpeeds = sizeof(speeds) / sizeof(speeds[0]);

    printf("[MOTOR_THREAD] Started (PID: %d)\n", getpid());

    /* Start motor in speed mode */
    MOTOR_CONTROL_SetMode(MOTOR_MODE_SPEED);
    MOTOR_CONTROL_SetSpeed(speeds[1]);  /* Start at 25% */
    MOTOR_CONTROL_Start();

    printf("[MOTOR_THREAD] Motor started at 25%% speed\n");

    while (g_running) {
        /* Cycle through speed levels every 5 seconds */
        sleep(5);

        if (!g_running)
            break;

        speedLevel = (speedLevel + 1) % numSpeeds;
        uint32_t newSpeed = speeds[speedLevel];

        MOTOR_CONTROL_SetSpeed(newSpeed);
        printf("[MOTOR_THREAD] Speed changed to %u%%\n", newSpeed);

        /* Demonstrate mode switching */
        if (speedLevel == 0) {
            MOTOR_CONTROL_Stop();
            printf("[MOTOR_THREAD] Motor stopped\n");
        } else if (speedLevel == 1) {
            MOTOR_CONTROL_Start();
            printf("[MOTOR_THREAD] Motor restarted\n");
        }
    }

    MOTOR_CONTROL_Stop();
    printf("[MOTOR_THREAD] Exiting...\n");
    return NULL;
}

/**
 * @brief System monitoring thread
 */
static void* SystemMonitorTask(void* arg)
{
    (void)arg;
    uint32_t uptimeSeconds = 0;

    printf("[MONITOR_THREAD] Started (PID: %d)\n", getpid());

    while (g_running) {
        /* Report every 10 seconds */
        sleep(10);

        if (!g_running)
            break;

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
            printf("[MONITOR] System Tick: %llu ms\n",
                   (unsigned long long)HAL_TIMER_GetSystemTickMs());
            printf("[MONITOR] =====================================\n\n");
        }

        /* Get sensor sample count */
        uint32_t sampleCount = SENSOR_MANAGER_GetSampleCount();
        printf("[MONITOR] Total sensor samples: %u\n", sampleCount);
    }

    printf("[MONITOR_THREAD] Exiting...\n");
    return NULL;
}
