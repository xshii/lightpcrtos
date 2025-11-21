/**
 * @file test_sim_timer.cpp
 * @brief Timer Simulator Unit Tests
 */

#include <gtest/gtest.h>

extern "C" {
#include "hal_timer.h"
#include "sim_timer.h"
}

class SimTimerTest : public ::testing::Test {
protected:
    void SetUp() override {
        SIM_TIMER_Init();
    }

    void TearDown() override {
        SIM_TIMER_Reset();
    }
};

TEST_F(SimTimerTest, Initialize) {
    EXPECT_EQ(HAL_OK, HAL_TIMER_Init());
}

TEST_F(SimTimerTest, CreateTimer) {
    TimerConfig config = {
        .mode = TIMER_MODE_PERIODIC,
        .periodUs = 1000,
        .callback = nullptr,
        .userData = nullptr,
        .priority = 0
    };

    TimerHandle handle;
    int ret = HAL_TIMER_Create(0, &config, &handle);

    EXPECT_EQ(HAL_OK, ret);
    EXPECT_NE(nullptr, handle);
}

TEST_F(SimTimerTest, StartStopTimer) {
    TimerConfig config = {
        .mode = TIMER_MODE_PERIODIC,
        .periodUs = 1000,
        .callback = nullptr,
        .userData = nullptr,
        .priority = 0
    };

    TimerHandle handle;
    HAL_TIMER_Create(0, &config, &handle);

    EXPECT_EQ(HAL_OK, HAL_TIMER_Start(handle));

    bool isRunning;
    HAL_TIMER_IsRunning(handle, &isRunning);
    EXPECT_TRUE(isRunning);

    EXPECT_EQ(HAL_OK, HAL_TIMER_Stop(handle));

    HAL_TIMER_IsRunning(handle, &isRunning);
    EXPECT_FALSE(isRunning);

    HAL_TIMER_Destroy(handle);
}

TEST_F(SimTimerTest, TimerCounter) {
    TimerConfig config = {
        .mode = TIMER_MODE_PERIODIC,
        .periodUs = 10000,
        .callback = nullptr,
        .userData = nullptr,
        .priority = 0
    };

    TimerHandle handle;
    HAL_TIMER_Create(0, &config, &handle);
    HAL_TIMER_Start(handle);

    // Advance time by 5ms
    SIM_TIMER_AdvanceTime(5000);

    uint32_t counter;
    HAL_TIMER_GetCounter(handle, &counter);
    EXPECT_EQ(5000u, counter);

    HAL_TIMER_Destroy(handle);
}

TEST_F(SimTimerTest, PeriodicTimerCallback) {
    int callbackCount = 0;

    auto callback = [](TimerHandle handle, void* userData) {
        int* count = static_cast<int*>(userData);
        (*count)++;
    };

    TimerConfig config = {
        .mode = TIMER_MODE_PERIODIC,
        .periodUs = 1000,  // 1ms period
        .callback = callback,
        .userData = &callbackCount,
        .priority = 0
    };

    TimerHandle handle;
    HAL_TIMER_Create(0, &config, &handle);
    HAL_TIMER_Start(handle);

    // Advance time by 5ms - should fire 5 times
    SIM_TIMER_AdvanceTime(5000);

    EXPECT_EQ(5, callbackCount);

    HAL_TIMER_Destroy(handle);
}

TEST_F(SimTimerTest, OneShotTimer) {
    int callbackCount = 0;

    auto callback = [](TimerHandle handle, void* userData) {
        int* count = static_cast<int*>(userData);
        (*count)++;
    };

    TimerConfig config = {
        .mode = TIMER_MODE_ONESHOT,
        .periodUs = 1000,
        .callback = callback,
        .userData = &callbackCount,
        .priority = 0
    };

    TimerHandle handle;
    HAL_TIMER_Create(0, &config, &handle);
    HAL_TIMER_Start(handle);

    // Advance time by 5ms - should fire only once
    SIM_TIMER_AdvanceTime(5000);

    EXPECT_EQ(1, callbackCount);

    // Timer should have stopped
    bool isRunning;
    HAL_TIMER_IsRunning(handle, &isRunning);
    EXPECT_FALSE(isRunning);

    HAL_TIMER_Destroy(handle);
}

TEST_F(SimTimerTest, MultipleTimers) {
    int count1 = 0, count2 = 0, count3 = 0;

    auto callback = [](TimerHandle handle, void* userData) {
        int* count = static_cast<int*>(userData);
        (*count)++;
    };

    TimerConfig config1 = {TIMER_MODE_PERIODIC, 1000, callback, &count1, 0};
    TimerConfig config2 = {TIMER_MODE_PERIODIC, 2000, callback, &count2, 0};
    TimerConfig config3 = {TIMER_MODE_PERIODIC, 5000, callback, &count3, 0};

    TimerHandle handle1, handle2, handle3;
    HAL_TIMER_Create(0, &config1, &handle1);
    HAL_TIMER_Create(1, &config2, &handle2);
    HAL_TIMER_Create(2, &config3, &handle3);

    HAL_TIMER_Start(handle1);
    HAL_TIMER_Start(handle2);
    HAL_TIMER_Start(handle3);

    // Advance time by 10ms
    SIM_TIMER_AdvanceTime(10000);

    EXPECT_EQ(10, count1);  // Every 1ms
    EXPECT_EQ(5, count2);   // Every 2ms
    EXPECT_EQ(2, count3);   // Every 5ms

    HAL_TIMER_Destroy(handle1);
    HAL_TIMER_Destroy(handle2);
    HAL_TIMER_Destroy(handle3);
}

TEST_F(SimTimerTest, ResetTimer) {
    TimerConfig config = {
        .mode = TIMER_MODE_PERIODIC,
        .periodUs = 10000,
        .callback = nullptr,
        .userData = nullptr,
        .priority = 0
    };

    TimerHandle handle;
    HAL_TIMER_Create(0, &config, &handle);
    HAL_TIMER_Start(handle);

    SIM_TIMER_AdvanceTime(5000);

    uint32_t counter;
    HAL_TIMER_GetCounter(handle, &counter);
    EXPECT_EQ(5000u, counter);

    HAL_TIMER_Reset(handle);

    HAL_TIMER_GetCounter(handle, &counter);
    EXPECT_EQ(0u, counter);

    HAL_TIMER_Destroy(handle);
}

TEST_F(SimTimerTest, SetPeriod) {
    int callbackCount = 0;

    auto callback = [](TimerHandle handle, void* userData) {
        int* count = static_cast<int*>(userData);
        (*count)++;
    };

    TimerConfig config = {
        .mode = TIMER_MODE_PERIODIC,
        .periodUs = 1000,
        .callback = callback,
        .userData = &callbackCount,
        .priority = 0
    };

    TimerHandle handle;
    HAL_TIMER_Create(0, &config, &handle);
    HAL_TIMER_Start(handle);

    // Fire at 1ms period for 5ms
    SIM_TIMER_AdvanceTime(5000);
    EXPECT_EQ(5, callbackCount);

    // Change to 2ms period
    HAL_TIMER_SetPeriod(handle, 2000);

    // Fire at 2ms period for 10ms
    SIM_TIMER_AdvanceTime(10000);
    EXPECT_EQ(5 + 5, callbackCount);

    HAL_TIMER_Destroy(handle);
}

TEST_F(SimTimerTest, SystemTick) {
    uint64_t tick1 = HAL_TIMER_GetSystemTickUs();
    EXPECT_EQ(0u, tick1);

    SIM_TIMER_AdvanceTime(1000000);  // 1 second

    uint64_t tick2 = HAL_TIMER_GetSystemTickUs();
    EXPECT_EQ(1000000u, tick2);

    uint64_t tickMs = HAL_TIMER_GetSystemTickMs();
    EXPECT_EQ(1000u, tickMs);
}

TEST_F(SimTimerTest, Delay) {
    uint64_t start = HAL_TIMER_GetSystemTickUs();

    HAL_TIMER_DelayUs(1000);
    uint64_t after1 = HAL_TIMER_GetSystemTickUs();
    EXPECT_EQ(1000u, after1 - start);

    HAL_TIMER_DelayMs(5);
    uint64_t after2 = HAL_TIMER_GetSystemTickUs();
    EXPECT_EQ(6000u, after2 - start);
}

TEST_F(SimTimerTest, GetStats) {
    uint32_t total, active, callbacks;

    TimerConfig config = {TIMER_MODE_PERIODIC, 1000, nullptr, nullptr, 0};

    TimerHandle h1, h2, h3;
    HAL_TIMER_Create(0, &config, &h1);
    HAL_TIMER_Create(1, &config, &h2);
    HAL_TIMER_Create(2, &config, &h3);

    SIM_TIMER_GetStats(&total, &active, &callbacks);
    EXPECT_EQ(3u, total);
    EXPECT_EQ(0u, active);  // None running yet

    HAL_TIMER_Start(h1);
    HAL_TIMER_Start(h2);

    active = SIM_TIMER_GetActiveTimerCount();
    EXPECT_EQ(2u, active);

    HAL_TIMER_Destroy(h1);
    HAL_TIMER_Destroy(h2);
    HAL_TIMER_Destroy(h3);
}

TEST_F(SimTimerTest, DestroyRunningTimer) {
    int callbackCount = 0;

    auto callback = [](TimerHandle handle, void* userData) {
        int* count = static_cast<int*>(userData);
        (*count)++;
    };

    TimerConfig config = {
        .mode = TIMER_MODE_PERIODIC,
        .periodUs = 1000,
        .callback = callback,
        .userData = &callbackCount,
        .priority = 0
    };

    TimerHandle handle;
    HAL_TIMER_Create(0, &config, &handle);
    HAL_TIMER_Start(handle);

    SIM_TIMER_AdvanceTime(3000);
    EXPECT_EQ(3, callbackCount);

    // Destroy while running
    HAL_TIMER_Destroy(handle);

    // Advance time - no more callbacks
    SIM_TIMER_AdvanceTime(5000);
    EXPECT_EQ(3, callbackCount);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
