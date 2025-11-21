/**
 * @file sim_dma.c
 * @brief DMA Simulation Implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hal_dma.h"

#define MAX_DMA_INSTANCES 8
#define MAX_DMA_CHANNELS 32

/* DMA instance state */
typedef struct {
    DmaId id;
    bool initialized;
} SimDmaInstance;

/* DMA channel state */
typedef struct {
    DmaChannel handle;
    DmaId dmaId;
    bool allocated;
    DmaDirection direction;
    uint32_t priority;
    DmaCallback callback;
    void* userData;
    bool busy;
    size_t bytesTransferred;
} SimDmaChannel;

/* Global state */
static struct {
    SimDmaInstance instances[MAX_DMA_INSTANCES];
    SimDmaChannel channels[MAX_DMA_CHANNELS];
    uint32_t nextHandle;
} g_simDma = {0};

/* HAL interface implementation */
int HAL_DMA_Init(DmaId dmaId, DmaConfig config)
{
    (void) config;

    if (dmaId >= MAX_DMA_INSTANCES)
        return HAL_ERROR;

    g_simDma.instances[dmaId].id = dmaId;
    g_simDma.instances[dmaId].initialized = true;

    printf("[SIM_DMA] Initialized DMA %u\n", dmaId);
    return HAL_OK;
}

int HAL_DMA_Deinit(DmaId dmaId)
{
    if (dmaId >= MAX_DMA_INSTANCES)
        return HAL_ERROR;

    g_simDma.instances[dmaId].initialized = false;

    printf("[SIM_DMA] Deinitialized DMA %u\n", dmaId);
    return HAL_OK;
}

int HAL_DMA_RequestChannel(DmaId dmaId, DmaDirection direction, uint32_t priority,
                           DmaChannel* channel)
{
    if (!channel || dmaId >= MAX_DMA_INSTANCES)
        return HAL_ERROR;
    if (!g_simDma.instances[dmaId].initialized)
        return HAL_ERROR;

    /* Find free channel */
    for (int i = 0; i < MAX_DMA_CHANNELS; i++) {
        if (!g_simDma.channels[i].allocated) {
            g_simDma.channels[i].handle = (DmaChannel) (uintptr_t) (++g_simDma.nextHandle);
            g_simDma.channels[i].dmaId = dmaId;
            g_simDma.channels[i].direction = direction;
            g_simDma.channels[i].priority = priority;
            g_simDma.channels[i].allocated = true;
            g_simDma.channels[i].busy = false;
            g_simDma.channels[i].bytesTransferred = 0;

            *channel = g_simDma.channels[i].handle;

            printf("[SIM_DMA] Requested channel %p on DMA %u\n", *channel, dmaId);
            return HAL_OK;
        }
    }

    return HAL_ERROR;
}

int HAL_DMA_ReleaseChannel(DmaChannel channel)
{
    for (int i = 0; i < MAX_DMA_CHANNELS; i++) {
        if (g_simDma.channels[i].allocated && g_simDma.channels[i].handle == channel) {
            g_simDma.channels[i].allocated = false;

            printf("[SIM_DMA] Released channel %p\n", channel);
            return HAL_OK;
        }
    }

    return HAL_ERROR;
}

int HAL_DMA_Configure(DmaChannel channel, DmaConfig config)
{
    (void) channel;
    (void) config;
    /* Configuration stored in opaque config */
    return HAL_OK;
}

int HAL_DMA_StartTransfer(DmaChannel channel, const void* srcAddr, void* dstAddr, size_t size)
{
    for (int i = 0; i < MAX_DMA_CHANNELS; i++) {
        if (g_simDma.channels[i].allocated && g_simDma.channels[i].handle == channel) {
            /* Simulate DMA transfer */
            memcpy(dstAddr, srcAddr, size);

            g_simDma.channels[i].busy = false;
            g_simDma.channels[i].bytesTransferred = size;

            printf("[SIM_DMA] Transfer complete: %zu bytes\n", size);

            /* Call callback if registered */
            if (g_simDma.channels[i].callback) {
                g_simDma.channels[i].callback(channel, DMA_EVENT_TRANSFER_COMPLETE,
                                              g_simDma.channels[i].userData);
            }

            return HAL_OK;
        }
    }

    return HAL_ERROR;
}

int HAL_DMA_StopTransfer(DmaChannel channel)
{
    for (int i = 0; i < MAX_DMA_CHANNELS; i++) {
        if (g_simDma.channels[i].allocated && g_simDma.channels[i].handle == channel) {
            g_simDma.channels[i].busy = false;

            printf("[SIM_DMA] Stopped transfer on channel %p\n", channel);
            return HAL_OK;
        }
    }

    return HAL_ERROR;
}

int HAL_DMA_IsBusy(DmaChannel channel, bool* isBusy)
{
    if (!isBusy)
        return HAL_ERROR;

    for (int i = 0; i < MAX_DMA_CHANNELS; i++) {
        if (g_simDma.channels[i].allocated && g_simDma.channels[i].handle == channel) {
            *isBusy = g_simDma.channels[i].busy;
            return HAL_OK;
        }
    }

    return HAL_ERROR;
}

int HAL_DMA_WaitComplete(DmaChannel channel, uint32_t timeoutMs)
{
    (void) timeoutMs;

    /* In simulation, transfers complete immediately */
    bool isBusy = false;
    if (HAL_DMA_IsBusy(channel, &isBusy) == HAL_OK) {
        return isBusy ? HAL_BUSY : HAL_OK;
    }

    return HAL_ERROR;
}

int HAL_DMA_RegisterCallback(DmaChannel channel, DmaCallback callback, void* userData)
{
    for (int i = 0; i < MAX_DMA_CHANNELS; i++) {
        if (g_simDma.channels[i].allocated && g_simDma.channels[i].handle == channel) {
            g_simDma.channels[i].callback = callback;
            g_simDma.channels[i].userData = userData;

            return HAL_OK;
        }
    }

    return HAL_ERROR;
}

int HAL_DMA_EnableEvents(DmaChannel channel, uint32_t events)
{
    (void) channel;
    (void) events;
    return HAL_OK;
}

int HAL_DMA_GetProgress(DmaChannel channel, size_t* bytesTransferred)
{
    if (!bytesTransferred)
        return HAL_ERROR;

    for (int i = 0; i < MAX_DMA_CHANNELS; i++) {
        if (g_simDma.channels[i].allocated && g_simDma.channels[i].handle == channel) {
            *bytesTransferred = g_simDma.channels[i].bytesTransferred;
            return HAL_OK;
        }
    }

    return HAL_ERROR;
}
