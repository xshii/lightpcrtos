/**
 * @file test_sensor_manager.cpp
 * @brief Sensor Manager Unit Tests
 */

#include <gtest/gtest.h>

extern "C" {
#include "sensor_manager.h"
#include "sim_memory.h"
}

class SensorManagerTest : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        // Initialize simulator
        SIM_MEMORY_SimulatorInit();

        // Configure memory pools
        SIM_MEMORY_ConfigurePool(POOL_NAME_SRAM, (void*) 0x20000000,
                                 1024 * 1024);  // 1MB SRAM
    }

    void TearDown() override
    {
        SENSOR_MANAGER_Deinit();
        SIM_MEMORY_SimulatorReset();
    }
};

TEST_F(SensorManagerTest, InitializeSuccessfully)
{
    int ret = SENSOR_MANAGER_Init();
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(SensorManagerTest, ReadSensorData)
{
    SENSOR_MANAGER_Init();

    SensorData data;
    int ret = SENSOR_MANAGER_Read(&data);

    EXPECT_EQ(HAL_OK, ret);
    EXPECT_TRUE(data.valid);
    EXPECT_GT(data.temperature, 0.0f);
}

TEST_F(SensorManagerTest, StartSampling)
{
    SENSOR_MANAGER_Init();

    int ret = SENSOR_MANAGER_StartSampling(100);  // 100ms period
    EXPECT_EQ(HAL_OK, ret);

    SENSOR_MANAGER_StopSampling();
}

TEST_F(SensorManagerTest, SampleCount)
{
    SENSOR_MANAGER_Init();

    uint32_t count1 = SENSOR_MANAGER_GetSampleCount();

    SensorData data;
    SENSOR_MANAGER_Read(&data);

    uint32_t count2 = SENSOR_MANAGER_GetSampleCount();

    EXPECT_EQ(count1 + 1, count2);
}

TEST_F(SensorManagerTest, MemoryAllocation)
{
    // Test that memory is allocated from correct pool
    uint32_t allocsBefore;
    size_t usageBefore;
    SIM_MEMORY_GetPoolStats(POOL_NAME_SRAM, &allocsBefore, &usageBefore);

    SENSOR_MANAGER_Init();

    uint32_t allocsAfter;
    size_t usageAfter;
    SIM_MEMORY_GetPoolStats(POOL_NAME_SRAM, &allocsAfter, &usageAfter);

    EXPECT_GT(allocsAfter, allocsBefore);
    EXPECT_GT(usageAfter, usageBefore);
}

TEST_F(SensorManagerTest, MultipleReads)
{
    SENSOR_MANAGER_Init();

    SensorData data1, data2, data3;
    EXPECT_EQ(HAL_OK, SENSOR_MANAGER_Read(&data1));
    EXPECT_EQ(HAL_OK, SENSOR_MANAGER_Read(&data2));
    EXPECT_EQ(HAL_OK, SENSOR_MANAGER_Read(&data3));

    EXPECT_TRUE(data1.valid);
    EXPECT_TRUE(data2.valid);
    EXPECT_TRUE(data3.valid);

    EXPECT_EQ(3u, SENSOR_MANAGER_GetSampleCount());
}

TEST_F(SensorManagerTest, ReadWithoutInit)
{
    SensorData data;
    int ret = SENSOR_MANAGER_Read(&data);
    EXPECT_EQ(HAL_ERROR, ret);
}

TEST_F(SensorManagerTest, ReadNullPointer)
{
    SENSOR_MANAGER_Init();
    int ret = SENSOR_MANAGER_Read(nullptr);
    EXPECT_EQ(HAL_ERROR, ret);
}

TEST_F(SensorManagerTest, DoubleInit)
{
    EXPECT_EQ(HAL_OK, SENSOR_MANAGER_Init());
    EXPECT_EQ(HAL_OK, SENSOR_MANAGER_Init());
}

TEST_F(SensorManagerTest, DeinitAndReinit)
{
    SENSOR_MANAGER_Init();
    EXPECT_EQ(HAL_OK, SENSOR_MANAGER_Deinit());
    EXPECT_EQ(HAL_OK, SENSOR_MANAGER_Init());
}

TEST_F(SensorManagerTest, SamplingLifecycle)
{
    SENSOR_MANAGER_Init();

    EXPECT_EQ(HAL_OK, SENSOR_MANAGER_StartSampling(50));
    EXPECT_EQ(HAL_OK, SENSOR_MANAGER_StopSampling());
    EXPECT_EQ(HAL_OK, SENSOR_MANAGER_StartSampling(100));
    EXPECT_EQ(HAL_OK, SENSOR_MANAGER_StopSampling());
}

TEST_F(SensorManagerTest, StartSamplingWithoutInit)
{
    int ret = SENSOR_MANAGER_StartSampling(100);
    EXPECT_EQ(HAL_ERROR, ret);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
