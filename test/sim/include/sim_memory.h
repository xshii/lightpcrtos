/**
 * @file sim_memory.h
 * @brief Memory Simulation - Implements HAL_MEMORY interface
 */

#ifndef SIM_MEMORY_H
#define SIM_MEMORY_H

#include "hal_memory.h"

/**
 * @brief Initialize memory simulator
 * @return 0 on success, -1 on failure
 */
int SIM_MEMORY_SimulatorInit(void);

/**
 * @brief Reset memory simulator
 * @return 0 on success, -1 on failure
 */
int SIM_MEMORY_SimulatorReset(void);

/**
 * @brief Configure cache address range for simulation
 * @param poolName Pool name
 * @param baseAddr Base address
 * @param size Total size
 * @return 0 on success, -1 on failure
 */
int SIM_MEMORY_ConfigurePool(PoolName poolName, void* baseAddr, size_t size);

/**
 * @brief Get allocation statistics
 * @param poolName Pool name
 * @param totalAllocs Output: total allocations
 * @param currentUsage Output: current memory usage
 * @return 0 on success, -1 on failure
 */
int SIM_MEMORY_GetPoolStats(PoolName poolName, uint32_t* totalAllocs, size_t* currentUsage);

#endif /* SIM_MEMORY_H */
