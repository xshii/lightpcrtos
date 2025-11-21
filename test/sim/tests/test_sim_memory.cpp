/**
 * @file test_sim_memory.cpp
 * @brief Memory Simulator Unit Tests
 */

#include <gtest/gtest.h>

extern "C" {
#include "hal_memory.h"
#include "sim_memory.h"
}

class SimMemoryTest : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        SIM_MEMORY_SimulatorInit();
    }

    void TearDown() override
    {
        SIM_MEMORY_SimulatorReset();
    }
};

TEST_F(SimMemoryTest, InitializeSimulator)
{
    int ret = SIM_MEMORY_SimulatorInit();
    EXPECT_EQ(0, ret);
}

TEST_F(SimMemoryTest, ConfigurePool)
{
    void* baseAddr = (void*) 0x10000000;
    size_t size = 1024 * 1024;  // 1MB

    int ret = SIM_MEMORY_ConfigurePool(POOL_NAME_L1, baseAddr, size);
    EXPECT_EQ(0, ret);
}

TEST_F(SimMemoryTest, ConfigureMultiplePools)
{
    int ret;

    ret = SIM_MEMORY_ConfigurePool(POOL_NAME_L1, (void*) 0x10000000, 256 * 1024);
    EXPECT_EQ(0, ret);

    ret = SIM_MEMORY_ConfigurePool(POOL_NAME_L2, (void*) 0x20000000, 512 * 1024);
    EXPECT_EQ(0, ret);

    ret = SIM_MEMORY_ConfigurePool(POOL_NAME_DDR, (void*) 0x80000000, 64 * 1024 * 1024);
    EXPECT_EQ(0, ret);
}

TEST_F(SimMemoryTest, AllocateFromPool)
{
    SIM_MEMORY_ConfigurePool(POOL_NAME_SRAM, (void*) 0x20000000, 1024 * 1024);

    MemoryBuffer buffer;
    int ret = HAL_MEMORY_AllocBuffer(POOL_NAME_SRAM, 1024, &buffer);

    EXPECT_EQ(HAL_OK, ret);
    EXPECT_NE(nullptr, buffer);
}

TEST_F(SimMemoryTest, AllocateFromUnconfiguredPool)
{
    // Don't configure the pool
    MemoryBuffer buffer;
    int ret = HAL_MEMORY_AllocBuffer(POOL_NAME_L1, 1024, &buffer);

    EXPECT_EQ(HAL_ERROR, ret);
}

TEST_F(SimMemoryTest, AllocateTooMuchMemory)
{
    size_t poolSize = 1024;
    SIM_MEMORY_ConfigurePool(POOL_NAME_SRAM, (void*) 0x20000000, poolSize);

    MemoryBuffer buffer;
    int ret = HAL_MEMORY_AllocBuffer(POOL_NAME_SRAM, poolSize * 2, &buffer);

    EXPECT_EQ(HAL_ERROR, ret);
}

TEST_F(SimMemoryTest, FreeBuffer)
{
    SIM_MEMORY_ConfigurePool(POOL_NAME_SRAM, (void*) 0x20000000, 1024 * 1024);

    MemoryBuffer buffer;
    HAL_MEMORY_AllocBuffer(POOL_NAME_SRAM, 1024, &buffer);

    int ret = HAL_MEMORY_FreeBuffer(buffer);
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(SimMemoryTest, GetBufferAddress)
{
    SIM_MEMORY_ConfigurePool(POOL_NAME_SRAM, (void*) 0x20000000, 1024 * 1024);

    MemoryBuffer buffer;
    HAL_MEMORY_AllocBuffer(POOL_NAME_SRAM, 1024, &buffer);

    void* addr = nullptr;
    int ret = HAL_MEMORY_GetAddr(buffer, &addr);

    EXPECT_EQ(HAL_OK, ret);
    EXPECT_NE(nullptr, addr);
}

TEST_F(SimMemoryTest, GetBufferInfo)
{
    SIM_MEMORY_ConfigurePool(POOL_NAME_L2, (void*) 0x20000000, 1024 * 1024);

    size_t allocSize = 2048;
    MemoryBuffer buffer;
    HAL_MEMORY_AllocBuffer(POOL_NAME_L2, allocSize, &buffer);

    MemoryBufferInfo info;
    int ret = HAL_MEMORY_GetBufferInfo(buffer, &info);

    EXPECT_EQ(HAL_OK, ret);
    EXPECT_STREQ(POOL_NAME_L2, info.poolName);
    EXPECT_EQ(allocSize, info.size);
    EXPECT_NE(nullptr, info.virtAddr);
}

TEST_F(SimMemoryTest, GetPoolStats)
{
    SIM_MEMORY_ConfigurePool(POOL_NAME_SRAM, (void*) 0x20000000, 1024 * 1024);

    uint32_t allocsBefore;
    size_t usageBefore;
    SIM_MEMORY_GetPoolStats(POOL_NAME_SRAM, &allocsBefore, &usageBefore);

    EXPECT_EQ(0u, allocsBefore);
    EXPECT_EQ(0u, usageBefore);

    MemoryBuffer buffer;
    HAL_MEMORY_AllocBuffer(POOL_NAME_SRAM, 1024, &buffer);

    uint32_t allocsAfter;
    size_t usageAfter;
    SIM_MEMORY_GetPoolStats(POOL_NAME_SRAM, &allocsAfter, &usageAfter);

    EXPECT_EQ(1u, allocsAfter);
    EXPECT_EQ(1024u, usageAfter);
}

TEST_F(SimMemoryTest, FlushBuffer)
{
    SIM_MEMORY_ConfigurePool(POOL_NAME_L1, (void*) 0x10000000, 1024 * 1024);

    MemoryBuffer buffer;
    HAL_MEMORY_AllocBuffer(POOL_NAME_L1, 1024, &buffer);

    int ret = HAL_MEMORY_FlushBuffer(buffer, 0, 0);
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(SimMemoryTest, InvalidateBuffer)
{
    SIM_MEMORY_ConfigurePool(POOL_NAME_L1, (void*) 0x10000000, 1024 * 1024);

    MemoryBuffer buffer;
    HAL_MEMORY_AllocBuffer(POOL_NAME_L1, 1024, &buffer);

    int ret = HAL_MEMORY_InvalidateBuffer(buffer, 0, 0);
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(SimMemoryTest, CopyBuffer)
{
    SIM_MEMORY_ConfigurePool(POOL_NAME_SRAM, (void*) 0x20000000, 1024 * 1024);

    MemoryBuffer srcBuffer, dstBuffer;
    HAL_MEMORY_AllocBuffer(POOL_NAME_SRAM, 1024, &srcBuffer);
    HAL_MEMORY_AllocBuffer(POOL_NAME_SRAM, 1024, &dstBuffer);

    // Write some data to source
    void* srcAddr;
    HAL_MEMORY_GetAddr(srcBuffer, &srcAddr);
    memset(srcAddr, 0xAA, 1024);

    // Copy
    int ret = HAL_MEMORY_CopyBuffer(dstBuffer, srcBuffer, 1024);
    EXPECT_EQ(HAL_OK, ret);

    // Verify
    void* dstAddr;
    HAL_MEMORY_GetAddr(dstBuffer, &dstAddr);
    EXPECT_EQ(0, memcmp(srcAddr, dstAddr, 1024));
}

TEST_F(SimMemoryTest, MultipleAllocationsAndFrees)
{
    SIM_MEMORY_ConfigurePool(POOL_NAME_SRAM, (void*) 0x20000000, 10 * 1024);

    MemoryBuffer buffers[10];

    // Allocate 10 buffers
    for (int i = 0; i < 10; i++) {
        int ret = HAL_MEMORY_AllocBuffer(POOL_NAME_SRAM, 1024, &buffers[i]);
        EXPECT_EQ(HAL_OK, ret);
    }

    // Check stats
    uint32_t allocs;
    size_t usage;
    SIM_MEMORY_GetPoolStats(POOL_NAME_SRAM, &allocs, &usage);
    EXPECT_EQ(10u, allocs);
    EXPECT_EQ(10 * 1024u, usage);

    // Free all buffers
    for (int i = 0; i < 10; i++) {
        HAL_MEMORY_FreeBuffer(buffers[i]);
    }

    // Check stats after free
    SIM_MEMORY_GetPoolStats(POOL_NAME_SRAM, &allocs, &usage);
    EXPECT_EQ(10u, allocs);  // alloc count doesn't decrease
    EXPECT_EQ(0u, usage);    // but usage should be 0
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
