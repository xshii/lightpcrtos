/**
 * @file sim_memory.c
 * @brief Memory Simulation Implementation
 */

#include "sim_memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_POOLS 16
#define MAX_BUFFERS 1024

/* Pool configuration */
typedef struct {
    PoolName name;
    void* baseAddr;
    size_t totalSize;
    size_t usedSize;
    uint32_t allocCount;
    bool configured;
} SimMemoryPool;

/* Buffer tracking */
typedef struct {
    MemoryBuffer handle;
    PoolName poolName;
    void* addr;
    size_t size;
    bool allocated;
} SimMemoryBuffer;

/* Global state */
static struct {
    bool initialized;
    SimMemoryPool pools[MAX_POOLS];
    SimMemoryBuffer buffers[MAX_BUFFERS];
    uint32_t nextHandle;
} g_simMemory = {0};

/* Private functions */
static SimMemoryPool* SimMemoryFindPool(PoolName poolName) {
    for (int i = 0; i < MAX_POOLS; i++) {
        if (g_simMemory.pools[i].configured &&
            strcmp(g_simMemory.pools[i].name, poolName) == 0) {
            return &g_simMemory.pools[i];
        }
    }
    return NULL;
}

static SimMemoryBuffer* SimMemoryFindFreeBuffer(void) {
    for (int i = 0; i < MAX_BUFFERS; i++) {
        if (!g_simMemory.buffers[i].allocated) {
            return &g_simMemory.buffers[i];
        }
    }
    return NULL;
}

/* Simulator control functions */
int SIM_MEMORY_SimulatorInit(void) {
    memset(&g_simMemory, 0, sizeof(g_simMemory));
    g_simMemory.initialized = true;
    g_simMemory.nextHandle = 1;

    printf("[SIM_MEMORY] Simulator initialized\n");
    return 0;
}

int SIM_MEMORY_SimulatorReset(void) {
    return SIM_MEMORY_SimulatorInit();
}

int SIM_MEMORY_ConfigurePool(PoolName poolName, void* baseAddr, size_t size) {
    if (!g_simMemory.initialized) return -1;

    /* Find free pool slot */
    for (int i = 0; i < MAX_POOLS; i++) {
        if (!g_simMemory.pools[i].configured) {
            g_simMemory.pools[i].name = poolName;
            g_simMemory.pools[i].baseAddr = baseAddr;
            g_simMemory.pools[i].totalSize = size;
            g_simMemory.pools[i].usedSize = 0;
            g_simMemory.pools[i].allocCount = 0;
            g_simMemory.pools[i].configured = true;

            printf("[SIM_MEMORY] Configured pool '%s': base=%p, size=%zu\n",
                   poolName, baseAddr, size);
            return 0;
        }
    }

    return -1;
}

int SIM_MEMORY_GetPoolStats(PoolName poolName, uint32_t* totalAllocs,
                            size_t* currentUsage) {
    SimMemoryPool* pool = SimMemoryFindPool(poolName);
    if (!pool) return -1;

    if (totalAllocs) *totalAllocs = pool->allocCount;
    if (currentUsage) *currentUsage = pool->usedSize;

    return 0;
}

/* HAL interface implementation */
int HAL_MEMORY_Init(void) {
    if (!g_simMemory.initialized) {
        return SIM_MEMORY_SimulatorInit();
    }
    return HAL_OK;
}

int HAL_MEMORY_Deinit(void) {
    return HAL_OK;
}

int HAL_MEMORY_AllocBuffer(PoolName poolName, size_t size,
                           MemoryBuffer* buffer) {
    if (!g_simMemory.initialized || !buffer) return HAL_ERROR;

    SimMemoryPool* pool = SimMemoryFindPool(poolName);
    if (!pool) {
        printf("[SIM_MEMORY] ERROR: Pool '%s' not configured\n", poolName);
        return HAL_ERROR;
    }

    if (pool->usedSize + size > pool->totalSize) {
        printf("[SIM_MEMORY] ERROR: Pool '%s' out of memory\n", poolName);
        return HAL_ERROR;
    }

    SimMemoryBuffer* buf = SimMemoryFindFreeBuffer();
    if (!buf) {
        printf("[SIM_MEMORY] ERROR: Too many buffers allocated\n");
        return HAL_ERROR;
    }

    /* Allocate actual memory */
    void* addr = malloc(size);
    if (!addr) return HAL_ERROR;

    buf->handle = (MemoryBuffer)(uintptr_t)g_simMemory.nextHandle++;
    buf->poolName = poolName;
    buf->addr = addr;
    buf->size = size;
    buf->allocated = true;

    pool->usedSize += size;
    pool->allocCount++;

    *buffer = buf->handle;

    printf("[SIM_MEMORY] Allocated %zu bytes from pool '%s' (handle=%p)\n",
           size, poolName, buf->handle);

    return HAL_OK;
}

int HAL_MEMORY_FreeBuffer(MemoryBuffer buffer) {
    if (!g_simMemory.initialized) return HAL_ERROR;

    for (int i = 0; i < MAX_BUFFERS; i++) {
        if (g_simMemory.buffers[i].allocated &&
            g_simMemory.buffers[i].handle == buffer) {

            SimMemoryPool* pool = SimMemoryFindPool(g_simMemory.buffers[i].poolName);
            if (pool) {
                pool->usedSize -= g_simMemory.buffers[i].size;
            }

            free(g_simMemory.buffers[i].addr);
            g_simMemory.buffers[i].allocated = false;

            printf("[SIM_MEMORY] Freed buffer %p\n", buffer);
            return HAL_OK;
        }
    }

    return HAL_ERROR;
}

int HAL_MEMORY_GetAddr(MemoryBuffer buffer, void** addr) {
    if (!g_simMemory.initialized || !addr) return HAL_ERROR;

    for (int i = 0; i < MAX_BUFFERS; i++) {
        if (g_simMemory.buffers[i].allocated &&
            g_simMemory.buffers[i].handle == buffer) {
            *addr = g_simMemory.buffers[i].addr;
            return HAL_OK;
        }
    }

    return HAL_ERROR;
}

int HAL_MEMORY_GetPhysAddr(MemoryBuffer buffer, void** physAddr) {
    /* In simulation, physical == virtual */
    return HAL_MEMORY_GetAddr(buffer, physAddr);
}

int HAL_MEMORY_GetBufferInfo(MemoryBuffer buffer, MemoryBufferInfo* info) {
    if (!g_simMemory.initialized || !info) return HAL_ERROR;

    for (int i = 0; i < MAX_BUFFERS; i++) {
        if (g_simMemory.buffers[i].allocated &&
            g_simMemory.buffers[i].handle == buffer) {

            info->poolName = g_simMemory.buffers[i].poolName;
            info->size = g_simMemory.buffers[i].size;
            info->virtAddr = g_simMemory.buffers[i].addr;
            info->physAddr = g_simMemory.buffers[i].addr;
            info->isCached = false;

            return HAL_OK;
        }
    }

    return HAL_ERROR;
}

int HAL_MEMORY_FlushBuffer(MemoryBuffer buffer, size_t offset, size_t size) {
    /* Simulation: no actual cache operation */
    (void)buffer;
    (void)offset;
    (void)size;
    printf("[SIM_MEMORY] Flush buffer %p (simulated)\n", buffer);
    return HAL_OK;
}

int HAL_MEMORY_InvalidateBuffer(MemoryBuffer buffer, size_t offset, size_t size) {
    /* Simulation: no actual cache operation */
    (void)buffer;
    (void)offset;
    (void)size;
    printf("[SIM_MEMORY] Invalidate buffer %p (simulated)\n", buffer);
    return HAL_OK;
}

int HAL_MEMORY_FlushAll(void) {
    printf("[SIM_MEMORY] Flush all caches (simulated)\n");
    return HAL_OK;
}

int HAL_MEMORY_InvalidateAll(void) {
    printf("[SIM_MEMORY] Invalidate all caches (simulated)\n");
    return HAL_OK;
}

int HAL_MEMORY_CopyBuffer(MemoryBuffer dstBuffer, MemoryBuffer srcBuffer,
                          size_t size) {
    void* dst = NULL;
    void* src = NULL;

    if (HAL_MEMORY_GetAddr(dstBuffer, &dst) != HAL_OK) return HAL_ERROR;
    if (HAL_MEMORY_GetAddr(srcBuffer, &src) != HAL_OK) return HAL_ERROR;

    memcpy(dst, src, size);

    printf("[SIM_MEMORY] Copied %zu bytes between buffers\n", size);
    return HAL_OK;
}
