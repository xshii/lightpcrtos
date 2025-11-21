/**
 * @file test_motor_control.cpp
 * @brief Motor Control Unit Tests
 */

#include <gtest/gtest.h>

extern "C" {
#include "motor_control.h"
#include "sim_memory.h"
}

class MotorControlTest : public ::testing::Test {
protected:
    void SetUp() override {
        SIM_MEMORY_SimulatorInit();

        // Configure memory pools
        SIM_MEMORY_ConfigurePool(POOL_NAME_L2,
                                 (void*)0x10000000,
                                 512 * 1024); // 512KB L2
    }

    void TearDown() override {
        MOTOR_CONTROL_Deinit();
        SIM_MEMORY_SimulatorReset();
    }
};

TEST_F(MotorControlTest, InitializeWithDma) {
    DmaId dmaId = 0;
    int ret = MOTOR_CONTROL_Init(dmaId);
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(MotorControlTest, SetMode) {
    MOTOR_CONTROL_Init(0);

    int ret = MOTOR_CONTROL_SetMode(MOTOR_MODE_SPEED);
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(MotorControlTest, SetSpeed) {
    MOTOR_CONTROL_Init(0);

    float targetSpeed = 1000.0f; // 1000 RPM
    int ret = MOTOR_CONTROL_SetSpeed(targetSpeed);
    EXPECT_EQ(HAL_OK, ret);

    MotorStatus status;
    MOTOR_CONTROL_GetStatus(&status);
    EXPECT_FLOAT_EQ(targetSpeed, status.targetSpeed);
}

TEST_F(MotorControlTest, StartStop) {
    MOTOR_CONTROL_Init(0);

    int ret = MOTOR_CONTROL_Start();
    EXPECT_EQ(HAL_OK, ret);

    MotorStatus status;
    MOTOR_CONTROL_GetStatus(&status);
    EXPECT_TRUE(status.isRunning);

    ret = MOTOR_CONTROL_Stop();
    EXPECT_EQ(HAL_OK, ret);

    MOTOR_CONTROL_GetStatus(&status);
    EXPECT_FALSE(status.isRunning);
}

TEST_F(MotorControlTest, SetModeWithoutInit) {
    int ret = MOTOR_CONTROL_SetMode(MOTOR_MODE_SPEED);
    EXPECT_EQ(HAL_ERROR, ret);
}

TEST_F(MotorControlTest, SetSpeedWithoutInit) {
    int ret = MOTOR_CONTROL_SetSpeed(1000.0f);
    EXPECT_EQ(HAL_ERROR, ret);
}

TEST_F(MotorControlTest, MultipleSpeedChanges) {
    MOTOR_CONTROL_Init(0);

    float speeds[] = {500.0f, 1000.0f, 1500.0f, 2000.0f};
    for (float speed : speeds) {
        EXPECT_EQ(HAL_OK, MOTOR_CONTROL_SetSpeed(speed));

        MotorStatus status;
        MOTOR_CONTROL_GetStatus(&status);
        EXPECT_FLOAT_EQ(speed, status.targetSpeed);
    }
}

TEST_F(MotorControlTest, DifferentModes) {
    MOTOR_CONTROL_Init(0);

    EXPECT_EQ(HAL_OK, MOTOR_CONTROL_SetMode(MOTOR_MODE_SPEED));
    EXPECT_EQ(HAL_OK, MOTOR_CONTROL_SetMode(MOTOR_MODE_POSITION));
    EXPECT_EQ(HAL_OK, MOTOR_CONTROL_SetMode(MOTOR_MODE_TORQUE));
}

TEST_F(MotorControlTest, StartStopMultipleTimes) {
    MOTOR_CONTROL_Init(0);

    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(HAL_OK, MOTOR_CONTROL_Start());
        EXPECT_EQ(HAL_OK, MOTOR_CONTROL_Stop());
    }
}

TEST_F(MotorControlTest, SpeedWhileRunning) {
    MOTOR_CONTROL_Init(0);
    MOTOR_CONTROL_Start();

    float newSpeed = 1500.0f;
    EXPECT_EQ(HAL_OK, MOTOR_CONTROL_SetSpeed(newSpeed));

    MotorStatus status;
    MOTOR_CONTROL_GetStatus(&status);
    EXPECT_FLOAT_EQ(newSpeed, status.targetSpeed);
    EXPECT_TRUE(status.isRunning);

    MOTOR_CONTROL_Stop();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
