/**
 * @file hal_dma.h
 * @brief DMA Hardware Abstraction Layer Interface
 */

#ifndef HAL_DMA_H
#define HAL_DMA_H

#include "hal_types.h"

/* DMA instance ID */
typedef uint32_t DmaId;

/* DMA channel handle */
typedef void* DmaChannel;

/* DMA configuration (opaque, platform-specific) */
typedef void* DmaConfig;

/* DMA transfer direction */
typedef enum {
    DMA_DIR_MEM_TO_MEM = 0,
    DMA_DIR_MEM_TO_PERIPH = 1,
    DMA_DIR_PERIPH_TO_MEM = 2
} DmaDirection;

/* DMA event types */
typedef enum {
    DMA_EVENT_TRANSFER_COMPLETE = (1 << 0),
    DMA_EVENT_TRANSFER_ERROR = (1 << 1),
    DMA_EVENT_HALF_COMPLETE = (1 << 2)
} DmaEvent;

/* DMA callback function */
typedef void (*DmaCallback)(DmaChannel channel, DmaEvent event, void* userData);

/**
 * @brief Initialize DMA controller
 * @param dmaId DMA instance ID
 * @param config Platform-specific configuration (can be NULL for default)
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_DMA_Init(DmaId dmaId, DmaConfig config);

/**
 * @brief Deinitialize DMA controller
 * @param dmaId DMA instance ID
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_DMA_Deinit(DmaId dmaId);

/**
 * @brief Request a DMA channel
 * @param dmaId DMA instance ID
 * @param direction Transfer direction
 * @param priority Channel priority (0=low, higher=more priority)
 * @param channel Output channel handle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_DMA_RequestChannel(DmaId dmaId, DmaDirection direction,
                           uint32_t priority, DmaChannel* channel);

/**
 * @brief Release a DMA channel
 * @param channel Channel handle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_DMA_ReleaseChannel(DmaChannel channel);

/**
 * @brief Configure DMA transfer parameters
 * @param channel Channel handle
 * @param config Platform-specific transfer configuration
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_DMA_Configure(DmaChannel channel, DmaConfig config);

/**
 * @brief Start DMA transfer
 * @param channel Channel handle
 * @param srcAddr Source address
 * @param dstAddr Destination address
 * @param size Transfer size in bytes
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_DMA_StartTransfer(DmaChannel channel, const void* srcAddr,
                          void* dstAddr, size_t size);

/**
 * @brief Stop DMA transfer
 * @param channel Channel handle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_DMA_StopTransfer(DmaChannel channel);

/**
 * @brief Check if DMA transfer is busy
 * @param channel Channel handle
 * @param isBusy Output: true if busy, false if idle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_DMA_IsBusy(DmaChannel channel, bool* isBusy);

/**
 * @brief Wait for DMA transfer completion
 * @param channel Channel handle
 * @param timeoutMs Timeout in milliseconds (0 = no timeout)
 * @return HAL_OK on success, HAL_TIMEOUT on timeout, HAL_ERROR on error
 */
int HAL_DMA_WaitComplete(DmaChannel channel, uint32_t timeoutMs);

/**
 * @brief Register DMA event callback
 * @param channel Channel handle
 * @param callback Callback function
 * @param userData User data passed to callback
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_DMA_RegisterCallback(DmaChannel channel, DmaCallback callback,
                             void* userData);

/**
 * @brief Enable DMA events
 * @param channel Channel handle
 * @param events Bitwise OR of DmaEvent values
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_DMA_EnableEvents(DmaChannel channel, uint32_t events);

/**
 * @brief Get DMA transfer progress
 * @param channel Channel handle
 * @param bytesTransferred Output: bytes transferred
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_DMA_GetProgress(DmaChannel channel, size_t* bytesTransferred);

#endif /* HAL_DMA_H */
