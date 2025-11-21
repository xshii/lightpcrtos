/**
 * @file hal_scheduler.h
 * @brief Graph Scheduler Hardware Abstraction Layer Interface
 */

#ifndef HAL_SCHEDULER_H
#define HAL_SCHEDULER_H

#include "hal_types.h"

/* Task handle */
typedef void* TaskHandle;

/* Task priority */
typedef enum {
    TASK_PRIORITY_LOW = 0,
    TASK_PRIORITY_NORMAL = 1,
    TASK_PRIORITY_HIGH = 2,
    TASK_PRIORITY_REALTIME = 3
} TaskPriority;

/* Task state */
typedef enum {
    TASK_STATE_READY = 0,
    TASK_STATE_RUNNING,
    TASK_STATE_BLOCKED,
    TASK_STATE_SUSPENDED,
    TASK_STATE_COMPLETED
} TaskState;

/* Task function pointer */
typedef void (*TaskFunction)(void* args);

/**
 * @brief Initialize scheduler
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_SCHEDULER_Init(void);

/**
 * @brief Create a task
 * @param taskFunc Task function pointer
 * @param args Task arguments
 * @param priority Task priority
 * @param taskHandle Output task handle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_SCHEDULER_CreateTask(TaskFunction taskFunc, void* args, TaskPriority priority,
                             TaskHandle* taskHandle);

/**
 * @brief Delete a task
 * @param taskHandle Task handle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_SCHEDULER_DeleteTask(TaskHandle taskHandle);

/**
 * @brief Start scheduler
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_SCHEDULER_Start(void);

/**
 * @brief Stop scheduler
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_SCHEDULER_Stop(void);

/**
 * @brief Get task state
 * @param taskHandle Task handle
 * @return Task state
 */
TaskState HAL_SCHEDULER_GetTaskState(TaskHandle taskHandle);

/**
 * @brief Yield current task
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_SCHEDULER_Yield(void);

/**
 * @brief Suspend a task
 * @param taskHandle Task handle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_SCHEDULER_SuspendTask(TaskHandle taskHandle);

/**
 * @brief Resume a task
 * @param taskHandle Task handle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_SCHEDULER_ResumeTask(TaskHandle taskHandle);

#endif /* HAL_SCHEDULER_H */
