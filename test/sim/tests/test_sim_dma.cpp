/**
 * @file test_sim_dma.cpp
 * @brief DMA Simulator Unit Tests
 */

#include <gtest/gtest.h>
#include <cstring>

extern "C" {
#include "hal_dma.h"
}

class SimDmaTest : public ::testing::Test {
protected:
    void SetUp() override {
        // DMA simulator is stateless, no special setup needed
    }

    void TearDown() override {
    }
};

TEST_F(SimDmaTest, InitializeDma) {
    DmaId dmaId = 0;
    int ret = HAL_DMA_Init(dmaId, nullptr);
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(SimDmaTest, InitializeMultipleDmas) {
    EXPECT_EQ(HAL_OK, HAL_DMA_Init(0, nullptr));
    EXPECT_EQ(HAL_OK, HAL_DMA_Init(1, nullptr));
    EXPECT_EQ(HAL_OK, HAL_DMA_Init(2, nullptr));
}

TEST_F(SimDmaTest, RequestChannel) {
    DmaId dmaId = 0;
    HAL_DMA_Init(dmaId, nullptr);

    DmaChannel channel;
    int ret = HAL_DMA_RequestChannel(dmaId, DMA_DIR_MEM_TO_MEM, 2, &channel);

    EXPECT_EQ(HAL_OK, ret);
    EXPECT_NE(nullptr, channel);
}

TEST_F(SimDmaTest, RequestMultipleChannels) {
    DmaId dmaId = 0;
    HAL_DMA_Init(dmaId, nullptr);

    DmaChannel channels[5];
    for (int i = 0; i < 5; i++) {
        int ret = HAL_DMA_RequestChannel(dmaId, DMA_DIR_MEM_TO_MEM, i, &channels[i]);
        EXPECT_EQ(HAL_OK, ret);
        EXPECT_NE(nullptr, channels[i]);
    }

    // All channels should have unique handles
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 5; j++) {
            EXPECT_NE(channels[i], channels[j]);
        }
    }
}

TEST_F(SimDmaTest, ReleaseChannel) {
    DmaId dmaId = 0;
    HAL_DMA_Init(dmaId, nullptr);

    DmaChannel channel;
    HAL_DMA_RequestChannel(dmaId, DMA_DIR_MEM_TO_MEM, 2, &channel);

    int ret = HAL_DMA_ReleaseChannel(channel);
    EXPECT_EQ(HAL_OK, ret);
}

TEST_F(SimDmaTest, StartTransfer) {
    DmaId dmaId = 0;
    HAL_DMA_Init(dmaId, nullptr);

    DmaChannel channel;
    HAL_DMA_RequestChannel(dmaId, DMA_DIR_MEM_TO_MEM, 2, &channel);

    uint8_t src[1024];
    uint8_t dst[1024];
    memset(src, 0xAA, sizeof(src));
    memset(dst, 0x00, sizeof(dst));

    int ret = HAL_DMA_StartTransfer(channel, src, dst, sizeof(src));
    EXPECT_EQ(HAL_OK, ret);

    // Verify transfer
    EXPECT_EQ(0, memcmp(src, dst, sizeof(src)));
}

TEST_F(SimDmaTest, TransferDifferentSizes) {
    DmaId dmaId = 0;
    HAL_DMA_Init(dmaId, nullptr);

    DmaChannel channel;
    HAL_DMA_RequestChannel(dmaId, DMA_DIR_MEM_TO_MEM, 2, &channel);

    size_t sizes[] = {64, 256, 1024, 4096, 16384};

    for (size_t size : sizes) {
        uint8_t* src = new uint8_t[size];
        uint8_t* dst = new uint8_t[size];

        memset(src, 0xBB, size);
        memset(dst, 0x00, size);

        int ret = HAL_DMA_StartTransfer(channel, src, dst, size);
        EXPECT_EQ(HAL_OK, ret);
        EXPECT_EQ(0, memcmp(src, dst, size));

        delete[] src;
        delete[] dst;
    }
}

TEST_F(SimDmaTest, IsBusy) {
    DmaId dmaId = 0;
    HAL_DMA_Init(dmaId, nullptr);

    DmaChannel channel;
    HAL_DMA_RequestChannel(dmaId, DMA_DIR_MEM_TO_MEM, 2, &channel);

    bool isBusy;
    int ret = HAL_DMA_IsBusy(channel, &isBusy);
    EXPECT_EQ(HAL_OK, ret);
    // In simulation, transfers complete immediately
    EXPECT_FALSE(isBusy);
}

TEST_F(SimDmaTest, GetProgress) {
    DmaId dmaId = 0;
    HAL_DMA_Init(dmaId, nullptr);

    DmaChannel channel;
    HAL_DMA_RequestChannel(dmaId, DMA_DIR_MEM_TO_MEM, 2, &channel);

    uint8_t src[1024];
    uint8_t dst[1024];
    size_t transferSize = sizeof(src);

    HAL_DMA_StartTransfer(channel, src, dst, transferSize);

    size_t bytesTransferred;
    int ret = HAL_DMA_GetProgress(channel, &bytesTransferred);
    EXPECT_EQ(HAL_OK, ret);
    EXPECT_EQ(transferSize, bytesTransferred);
}

TEST_F(SimDmaTest, RegisterCallback) {
    DmaId dmaId = 0;
    HAL_DMA_Init(dmaId, nullptr);

    DmaChannel channel;
    HAL_DMA_RequestChannel(dmaId, DMA_DIR_MEM_TO_MEM, 2, &channel);

    bool callbackCalled = false;
    auto callback = [](DmaChannel ch, DmaEvent event, void* userData) {
        bool* called = static_cast<bool*>(userData);
        *called = true;
        EXPECT_EQ(DMA_EVENT_TRANSFER_COMPLETE, event);
    };

    int ret = HAL_DMA_RegisterCallback(channel, callback, &callbackCalled);
    EXPECT_EQ(HAL_OK, ret);

    uint8_t src[64];
    uint8_t dst[64];
    HAL_DMA_StartTransfer(channel, src, dst, sizeof(src));

    EXPECT_TRUE(callbackCalled);
}

TEST_F(SimDmaTest, MultipleTransfersOnSameChannel) {
    DmaId dmaId = 0;
    HAL_DMA_Init(dmaId, nullptr);

    DmaChannel channel;
    HAL_DMA_RequestChannel(dmaId, DMA_DIR_MEM_TO_MEM, 2, &channel);

    for (int i = 0; i < 10; i++) {
        uint8_t src[256];
        uint8_t dst[256];
        memset(src, 0x55, sizeof(src));
        memset(dst, 0x00, sizeof(dst));

        int ret = HAL_DMA_StartTransfer(channel, src, dst, sizeof(src));
        EXPECT_EQ(HAL_OK, ret);
        EXPECT_EQ(0, memcmp(src, dst, sizeof(src)));
    }
}

TEST_F(SimDmaTest, DifferentDirections) {
    DmaId dmaId = 0;
    HAL_DMA_Init(dmaId, nullptr);

    DmaChannel channel1, channel2, channel3;

    EXPECT_EQ(HAL_OK, HAL_DMA_RequestChannel(dmaId, DMA_DIR_MEM_TO_MEM, 1, &channel1));
    EXPECT_EQ(HAL_OK, HAL_DMA_RequestChannel(dmaId, DMA_DIR_MEM_TO_PERIPH, 1, &channel2));
    EXPECT_EQ(HAL_OK, HAL_DMA_RequestChannel(dmaId, DMA_DIR_PERIPH_TO_MEM, 1, &channel3));

    EXPECT_NE(channel1, channel2);
    EXPECT_NE(channel2, channel3);
}

TEST_F(SimDmaTest, EnableEvents) {
    DmaId dmaId = 0;
    HAL_DMA_Init(dmaId, nullptr);

    DmaChannel channel;
    HAL_DMA_RequestChannel(dmaId, DMA_DIR_MEM_TO_MEM, 2, &channel);

    uint32_t events = DMA_EVENT_TRANSFER_COMPLETE | DMA_EVENT_TRANSFER_ERROR;
    int ret = HAL_DMA_EnableEvents(channel, events);
    EXPECT_EQ(HAL_OK, ret);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
