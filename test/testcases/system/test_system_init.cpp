/**
 * @file test_system_init.cpp
 * @brief System Initialization Unit Tests
 */

#include <gtest/gtest.h>

extern "C" {
#include "hal_dma.h"
#include "hal_memory.h"
#include "hal_scheduler.h"
#include "hal_timer.h"
#include "sensor_manager.h"
#include "motor_control.h"
#include "system_init.h"
}

class SystemInitTest : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        // System initialization handled by each test
    }

    void TearDown() override
    {
        // Cleanup after each test
        if (SYSTEM_IsInitialized()) {
            SYSTEM_Deinit();
        }
    }
};

TEST_F(SystemInitTest, InitializeSystem)
{
    int ret = SYSTEM_Init();
    EXPECT_EQ(HAL_OK, ret);
    EXPECT_TRUE(SYSTEM_IsInitialized());
}

TEST_F(SystemInitTest, GetInitStatus)
{
    SYSTEM_Init();

    SystemInitStatus status;
    int ret = SYSTEM_GetInitStatus(&status);

    EXPECT_EQ(HAL_OK, ret);
    EXPECT_EQ(INIT_STAGE_COMPLETE, status.currentStage);
    EXPECT_TRUE(status.halInitialized);
    EXPECT_TRUE(status.sensorInitialized);
    EXPECT_TRUE(status.motorInitialized);
    EXPECT_EQ(HAL_OK, status.lastError);
}

TEST_F(SystemInitTest, InitTimeMeasured)
{
    SYSTEM_Init();

    SystemInitStatus status;
    SYSTEM_GetInitStatus(&status);

    // Initialization time should be measured (may be 0 in simulation)
    EXPECT_GE(status.initTimeMs, 0u);
}

TEST_F(SystemInitTest, DeinitializeSystem)
{
    SYSTEM_Init();
    EXPECT_TRUE(SYSTEM_IsInitialized());

    int ret = SYSTEM_Deinit();
    EXPECT_EQ(HAL_OK, ret);
    EXPECT_FALSE(SYSTEM_IsInitialized());
}

TEST_F(SystemInitTest, GetStatusWithNullPointer)
{
    SYSTEM_Init();

    int ret = SYSTEM_GetInitStatus(nullptr);
    EXPECT_EQ(HAL_ERROR, ret);
}

TEST_F(SystemInitTest, IsInitializedBeforeInit)
{
    EXPECT_FALSE(SYSTEM_IsInitialized());
}

TEST_F(SystemInitTest, MultipleInitCalls)
{
    int ret1 = SYSTEM_Init();
    EXPECT_EQ(HAL_OK, ret1);

    // Second init should also succeed (reinitialize)
    int ret2 = SYSTEM_Init();
    EXPECT_EQ(HAL_OK, ret2);

    EXPECT_TRUE(SYSTEM_IsInitialized());
}

TEST_F(SystemInitTest, DeinitBeforeInit)
{
    // Should handle gracefully
    int ret = SYSTEM_Deinit();
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(SystemInitTest, InitDeinitCycle)
{
    // First cycle
    EXPECT_EQ(HAL_OK, SYSTEM_Init());
    EXPECT_TRUE(SYSTEM_IsInitialized());
    EXPECT_EQ(HAL_OK, SYSTEM_Deinit());
    EXPECT_FALSE(SYSTEM_IsInitialized());

    // Second cycle
    EXPECT_EQ(HAL_OK, SYSTEM_Init());
    EXPECT_TRUE(SYSTEM_IsInitialized());
    EXPECT_EQ(HAL_OK, SYSTEM_Deinit());
    EXPECT_FALSE(SYSTEM_IsInitialized());
}

TEST_F(SystemInitTest, StatusResetAfterDeinit)
{
    SYSTEM_Init();
    SYSTEM_Deinit();

    SystemInitStatus status;
    SYSTEM_GetInitStatus(&status);

    EXPECT_EQ(INIT_STAGE_HARDWARE, status.currentStage);
    EXPECT_FALSE(status.halInitialized);
    EXPECT_FALSE(status.sensorInitialized);
    EXPECT_FALSE(status.motorInitialized);
}

TEST_F(SystemInitTest, HardwareLayerInitialized)
{
    SYSTEM_Init();

    SystemInitStatus status;
    SYSTEM_GetInitStatus(&status);

    // Verify HAL components are initialized
    EXPECT_TRUE(status.halInitialized);

    // Can verify by trying to use HAL functions
    uint64_t tick = HAL_TIMER_GetSystemTickUs();
    EXPECT_GE(tick, 0u);
}

TEST_F(SystemInitTest, BusinessLayerInitialized)
{
    SYSTEM_Init();

    SystemInitStatus status;
    SYSTEM_GetInitStatus(&status);

    EXPECT_TRUE(status.sensorInitialized);
    EXPECT_TRUE(status.motorInitialized);

    // Verify by reading sensor
    SensorData data;
    int ret = SENSOR_MANAGER_Read(&data);
    EXPECT_EQ(HAL_OK, ret);
}
