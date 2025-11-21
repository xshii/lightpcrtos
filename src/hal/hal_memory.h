/**
 * @file hal_memory.h
 * @brief Memory Hardware Abstraction Layer Interface
 * @note Inspired by TI CMEM, FreeRTOS Cache HAL, and Qualcomm Buffer Management
 */

#ifndef HAL_MEMORY_H
#define HAL_MEMORY_H

#include "hal_types.h"

/* Memory buffer handle */
typedef void* MemoryBuffer;

/* Cache/Pool name (string identifier) */
typedef const char* PoolName;

/* Predefined pool names (extensible) */
#define POOL_NAME_L1       "L1"
#define POOL_NAME_L2       "L2"
#define POOL_NAME_L3       "L3"
#define POOL_NAME_SRAM     "SRAM"
#define POOL_NAME_DDR      "DDR"
#define POOL_NAME_TCM      "TCM"

/* Memory buffer attributes */
typedef struct {
    PoolName poolName;
    size_t size;
    void* virtAddr;      /* Virtual address */
    void* physAddr;      /* Physical address (if applicable) */
    bool isCached;
} MemoryBufferInfo;

/**
 * @brief Initialize memory subsystem
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_MEMORY_Init(void);

/**
 * @brief Deinitialize memory subsystem
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_MEMORY_Deinit(void);

/**
 * @brief Allocate buffer from named pool
 * @param poolName Pool name (e.g., "L1", "DDR")
 * @param size Size in bytes
 * @param buffer Output buffer handle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_MEMORY_AllocBuffer(PoolName poolName, size_t size,
                           MemoryBuffer* buffer);

/**
 * @brief Free allocated buffer
 * @param buffer Buffer handle
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_MEMORY_FreeBuffer(MemoryBuffer buffer);

/**
 * @brief Get buffer virtual address
 * @param buffer Buffer handle
 * @param addr Output virtual address
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_MEMORY_GetAddr(MemoryBuffer buffer, void** addr);

/**
 * @brief Get buffer physical address
 * @param buffer Buffer handle
 * @param physAddr Output physical address
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_MEMORY_GetPhysAddr(MemoryBuffer buffer, void** physAddr);

/**
 * @brief Get buffer information
 * @param buffer Buffer handle
 * @param info Output buffer info
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_MEMORY_GetBufferInfo(MemoryBuffer buffer, MemoryBufferInfo* info);

/**
 * @brief Flush buffer (write back cache to memory)
 * @param buffer Buffer handle
 * @param offset Offset within buffer (0 for entire buffer)
 * @param size Size to flush (0 for entire buffer)
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_MEMORY_FlushBuffer(MemoryBuffer buffer, size_t offset, size_t size);

/**
 * @brief Invalidate buffer (discard cache contents)
 * @param buffer Buffer handle
 * @param offset Offset within buffer (0 for entire buffer)
 * @param size Size to invalidate (0 for entire buffer)
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_MEMORY_InvalidateBuffer(MemoryBuffer buffer, size_t offset, size_t size);

/**
 * @brief Flush all data caches
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_MEMORY_FlushAll(void);

/**
 * @brief Invalidate all data caches
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_MEMORY_InvalidateAll(void);

/**
 * @brief Copy data between buffers (may use DMA if available)
 * @param dstBuffer Destination buffer
 * @param srcBuffer Source buffer
 * @param size Size in bytes
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int HAL_MEMORY_CopyBuffer(MemoryBuffer dstBuffer, MemoryBuffer srcBuffer,
                          size_t size);

#endif /* HAL_MEMORY_H */
