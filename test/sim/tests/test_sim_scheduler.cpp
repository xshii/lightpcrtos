/**
 * @file test_sim_scheduler.cpp
 * @brief Scheduler Simulator Unit Tests
 */

#include <gtest/gtest.h>

extern "C" {
#include "hal_scheduler.h"
}

class SimSchedulerTest : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        HAL_SCHEDULER_Init();
    }

    void TearDown() override {}
};

static void DummyTaskFunc(void* args)
{
    // Dummy task function
    (void) args;
}

TEST_F(SimSchedulerTest, Initialize)
{
    int ret = HAL_SCHEDULER_Init();
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(SimSchedulerTest, CreateTask)
{
    TaskHandle handle;
    int ret = HAL_SCHEDULER_CreateTask(DummyTaskFunc, nullptr, TASK_PRIORITY_NORMAL, &handle);
    EXPECT_EQ(HAL_OK, ret);
    EXPECT_NE(nullptr, handle);
}

TEST_F(SimSchedulerTest, CreateMultipleTasks)
{
    TaskHandle handles[5];

    for (int i = 0; i < 5; i++) {
        int ret =
            HAL_SCHEDULER_CreateTask(DummyTaskFunc, nullptr, TASK_PRIORITY_NORMAL, &handles[i]);
        EXPECT_EQ(HAL_OK, ret);
        EXPECT_NE(nullptr, handles[i]);
    }

    // All handles should be unique
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 5; j++) {
            EXPECT_NE(handles[i], handles[j]);
        }
    }
}

TEST_F(SimSchedulerTest, DeleteTask)
{
    TaskHandle handle;
    HAL_SCHEDULER_CreateTask(DummyTaskFunc, nullptr, TASK_PRIORITY_NORMAL, &handle);

    int ret = HAL_SCHEDULER_DeleteTask(handle);
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(SimSchedulerTest, GetTaskState)
{
    TaskHandle handle;
    HAL_SCHEDULER_CreateTask(DummyTaskFunc, nullptr, TASK_PRIORITY_NORMAL, &handle);

    TaskState state = HAL_SCHEDULER_GetTaskState(handle);
    EXPECT_EQ(TASK_STATE_READY, state);
}

TEST_F(SimSchedulerTest, SuspendTask)
{
    TaskHandle handle;
    HAL_SCHEDULER_CreateTask(DummyTaskFunc, nullptr, TASK_PRIORITY_NORMAL, &handle);

    int ret = HAL_SCHEDULER_SuspendTask(handle);
    EXPECT_EQ(HAL_OK, ret);

    TaskState state = HAL_SCHEDULER_GetTaskState(handle);
    EXPECT_EQ(TASK_STATE_SUSPENDED, state);
}

TEST_F(SimSchedulerTest, ResumeTask)
{
    TaskHandle handle;
    HAL_SCHEDULER_CreateTask(DummyTaskFunc, nullptr, TASK_PRIORITY_NORMAL, &handle);

    HAL_SCHEDULER_SuspendTask(handle);
    EXPECT_EQ(TASK_STATE_SUSPENDED, HAL_SCHEDULER_GetTaskState(handle));

    int ret = HAL_SCHEDULER_ResumeTask(handle);
    EXPECT_EQ(HAL_OK, ret);

    TaskState state = HAL_SCHEDULER_GetTaskState(handle);
    EXPECT_EQ(TASK_STATE_READY, state);
}

TEST_F(SimSchedulerTest, TaskPriorities)
{
    TaskHandle lowPrioTask, normalPrioTask, highPrioTask;

    HAL_SCHEDULER_CreateTask(DummyTaskFunc, nullptr, TASK_PRIORITY_LOW, &lowPrioTask);
    HAL_SCHEDULER_CreateTask(DummyTaskFunc, nullptr, TASK_PRIORITY_NORMAL, &normalPrioTask);
    HAL_SCHEDULER_CreateTask(DummyTaskFunc, nullptr, TASK_PRIORITY_HIGH, &highPrioTask);

    EXPECT_NE(lowPrioTask, normalPrioTask);
    EXPECT_NE(normalPrioTask, highPrioTask);
}

TEST_F(SimSchedulerTest, StartScheduler)
{
    int ret = HAL_SCHEDULER_Start();
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(SimSchedulerTest, StopScheduler)
{
    HAL_SCHEDULER_Start();
    int ret = HAL_SCHEDULER_Stop();
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(SimSchedulerTest, Yield)
{
    int ret = HAL_SCHEDULER_Yield();
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(SimSchedulerTest, TaskWithArguments)
{
    int taskArg = 42;
    TaskHandle handle;

    int ret = HAL_SCHEDULER_CreateTask(DummyTaskFunc, &taskArg, TASK_PRIORITY_NORMAL, &handle);
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(SimSchedulerTest, CreateAndDeleteMultipleTasks)
{
    TaskHandle handles[10];

    // Create 10 tasks
    for (int i = 0; i < 10; i++) {
        HAL_SCHEDULER_CreateTask(DummyTaskFunc, nullptr, TASK_PRIORITY_NORMAL, &handles[i]);
    }

    // Delete all tasks
    for (int i = 0; i < 10; i++) {
        int ret = HAL_SCHEDULER_DeleteTask(handles[i]);
        EXPECT_EQ(HAL_OK, ret);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
