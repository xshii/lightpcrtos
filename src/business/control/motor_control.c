/**
 * @file motor_control.c
 * @brief Motor Control Implementation
 */

#include "motor_control.h"

#include <string.h>

#include "hal_memory.h"

/* Module state */
static struct {
    bool initialized;
    MotorMode mode;
    float targetSpeed;
    float currentSpeed;
    float position;
    bool isRunning;
    DmaId dmaId;
    DmaChannel dmaChannel;
    MemoryBuffer controlBuffer;
} g_motorControl = {0};

/* Private functions */
static int MotorControlAllocateBuffer(void)
{
    return HAL_MEMORY_AllocBuffer(POOL_NAME_L2, 4096, &g_motorControl.controlBuffer);
}

static void MotorControlFreeBuffer(void)
{
    if (g_motorControl.controlBuffer) {
        HAL_MEMORY_FreeBuffer(g_motorControl.controlBuffer);
        g_motorControl.controlBuffer = NULL;
    }
}

static int MotorControlSetupDma(void)
{
    return HAL_DMA_RequestChannel(g_motorControl.dmaId, DMA_DIR_MEM_TO_PERIPH, 2, /* priority */
                                  &g_motorControl.dmaChannel);
}

/* Public functions */
int MOTOR_CONTROL_Init(DmaId dmaId)
{
    if (g_motorControl.initialized) {
        return HAL_OK;
    }

    g_motorControl.dmaId = dmaId;

    /* Initialize DMA */
    if (HAL_DMA_Init(dmaId, NULL) != HAL_OK) {
        return HAL_ERROR;
    }

    /* Setup DMA channel */
    if (MotorControlSetupDma() != HAL_OK) {
        return HAL_ERROR;
    }

    /* Allocate control buffer */
    if (MotorControlAllocateBuffer() != HAL_OK) {
        HAL_DMA_ReleaseChannel(g_motorControl.dmaChannel);
        return HAL_ERROR;
    }

    g_motorControl.initialized = true;
    g_motorControl.mode = MOTOR_MODE_SPEED;
    g_motorControl.isRunning = false;

    return HAL_OK;
}

int MOTOR_CONTROL_Deinit(void)
{
    if (!g_motorControl.initialized) {
        return HAL_OK;
    }

    /* Release DMA channel */
    if (g_motorControl.dmaChannel) {
        HAL_DMA_ReleaseChannel(g_motorControl.dmaChannel);
    }

    /* Free control buffer */
    MotorControlFreeBuffer();

    /* Reset state */
    memset(&g_motorControl, 0, sizeof(g_motorControl));

    return HAL_OK;
}

int MOTOR_CONTROL_SetMode(MotorMode mode)
{
    if (!g_motorControl.initialized) {
        return HAL_ERROR;
    }

    g_motorControl.mode = mode;
    return HAL_OK;
}

int MOTOR_CONTROL_SetSpeed(float speed)
{
    if (!g_motorControl.initialized) {
        return HAL_ERROR;
    }

    g_motorControl.targetSpeed = speed;

    /* TODO: Use DMA to transfer control data to motor controller */

    return HAL_OK;
}

int MOTOR_CONTROL_GetStatus(MotorStatus* status)
{
    if (!g_motorControl.initialized || !status) {
        return HAL_ERROR;
    }

    status->currentSpeed = g_motorControl.currentSpeed;
    status->targetSpeed = g_motorControl.targetSpeed;
    status->position = g_motorControl.position;
    status->isRunning = g_motorControl.isRunning;

    return HAL_OK;
}

int MOTOR_CONTROL_Start(void)
{
    if (!g_motorControl.initialized) {
        return HAL_ERROR;
    }

    g_motorControl.isRunning = true;
    return HAL_OK;
}

int MOTOR_CONTROL_Stop(void)
{
    if (!g_motorControl.initialized) {
        return HAL_ERROR;
    }

    g_motorControl.isRunning = false;
    g_motorControl.currentSpeed = 0.0f;

    return HAL_OK;
}
