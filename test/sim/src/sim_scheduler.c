/**
 * @file sim_scheduler.c
 * @brief Scheduler Simulation Implementation (Simplified)
 */

#include "hal_scheduler.h"
#include <stdlib.h>
#include <stdio.h>

#define MAX_TASKS 64

typedef struct {
    TaskHandle handle;
    TaskFunction func;
    void* args;
    TaskPriority priority;
    TaskState state;
    bool allocated;
} SimTask;

static struct {
    bool initialized;
    SimTask tasks[MAX_TASKS];
    uint32_t nextHandle;
} g_simScheduler = {0};

/* HAL interface implementation */
int HAL_SCHEDULER_Init(void) {
    g_simScheduler.initialized = true;
    g_simScheduler.nextHandle = 1;

    printf("[SIM_SCHEDULER] Initialized\n");
    return HAL_OK;
}

int HAL_SCHEDULER_CreateTask(TaskFunction taskFunc, void* args,
                              TaskPriority priority, TaskHandle* taskHandle) {
    if (!g_simScheduler.initialized || !taskFunc || !taskHandle) {
        return HAL_ERROR;
    }

    for (int i = 0; i < MAX_TASKS; i++) {
        if (!g_simScheduler.tasks[i].allocated) {
            g_simScheduler.tasks[i].handle = (TaskHandle)(uintptr_t)(g_simScheduler.nextHandle++);
            g_simScheduler.tasks[i].func = taskFunc;
            g_simScheduler.tasks[i].args = args;
            g_simScheduler.tasks[i].priority = priority;
            g_simScheduler.tasks[i].state = TASK_STATE_READY;
            g_simScheduler.tasks[i].allocated = true;

            *taskHandle = g_simScheduler.tasks[i].handle;

            printf("[SIM_SCHEDULER] Created task %p\n", *taskHandle);
            return HAL_OK;
        }
    }

    return HAL_ERROR;
}

int HAL_SCHEDULER_DeleteTask(TaskHandle taskHandle) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (g_simScheduler.tasks[i].allocated &&
            g_simScheduler.tasks[i].handle == taskHandle) {

            g_simScheduler.tasks[i].allocated = false;

            printf("[SIM_SCHEDULER] Deleted task %p\n", taskHandle);
            return HAL_OK;
        }
    }

    return HAL_ERROR;
}

int HAL_SCHEDULER_Start(void) {
    printf("[SIM_SCHEDULER] Started\n");
    return HAL_OK;
}

int HAL_SCHEDULER_Stop(void) {
    printf("[SIM_SCHEDULER] Stopped\n");
    return HAL_OK;
}

TaskState HAL_SCHEDULER_GetTaskState(TaskHandle taskHandle) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (g_simScheduler.tasks[i].allocated &&
            g_simScheduler.tasks[i].handle == taskHandle) {
            return g_simScheduler.tasks[i].state;
        }
    }

    return TASK_STATE_SUSPENDED;
}

int HAL_SCHEDULER_Yield(void) {
    return HAL_OK;
}

int HAL_SCHEDULER_SuspendTask(TaskHandle taskHandle) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (g_simScheduler.tasks[i].allocated &&
            g_simScheduler.tasks[i].handle == taskHandle) {

            g_simScheduler.tasks[i].state = TASK_STATE_SUSPENDED;
            return HAL_OK;
        }
    }

    return HAL_ERROR;
}

int HAL_SCHEDULER_ResumeTask(TaskHandle taskHandle) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (g_simScheduler.tasks[i].allocated &&
            g_simScheduler.tasks[i].handle == taskHandle) {

            g_simScheduler.tasks[i].state = TASK_STATE_READY;
            return HAL_OK;
        }
    }

    return HAL_ERROR;
}
