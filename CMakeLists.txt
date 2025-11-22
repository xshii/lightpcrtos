cmake_minimum_required(VERSION 3.15)
project(LightPCRTOS VERSION 1.0.0 LANGUAGES C CXX)

# ====================================
# 构建选项
# ====================================
option(BUILD_TESTS "Build unit tests (PC target)" OFF)
option(BUILD_EMBEDDED "Build embedded target (ARM cross-compile)" OFF)
option(BUILD_LINUX_APP "Build Linux application (simulation)" ON)
option(ENABLE_COVERAGE "Enable code coverage for tests" OFF)

# ====================================
# 全局设置
# ====================================
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ====================================
# 输出配置信息
# ====================================
message(STATUS "")
message(STATUS "========================================")
message(STATUS "  ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "========================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER}")

# ====================================
# 条件编译
# ====================================
if(BUILD_TESTS)
    message(STATUS "Target: PC Unit Tests")
    message(STATUS "Coverage: ${ENABLE_COVERAGE}")

    add_definitions(-DUNIT_TEST_BUILD)
    add_definitions(-DPC_PLATFORM)
    add_definitions(-DHARDWARE_SIMULATION)

    # 包含测试构建
    enable_testing()
    add_subdirectory(test)

elseif(BUILD_LINUX_APP)
    message(STATUS "Target: Linux Application (Simulation)")

    add_definitions(-DLINUX_PLATFORM)
    add_definitions(-DHARDWARE_SIMULATION)

    # 构建Linux模拟应用
    add_subdirectory(src)

elseif(BUILD_EMBEDDED)
    message(STATUS "Target: Embedded ARM (Production)")

    add_definitions(-DEMBEDDED_TARGET)
    add_definitions(-DARM_CORTEX_M4)

    # 构建嵌入式目标
    add_subdirectory(src)
endif()

message(STATUS "========================================")
message(STATUS "")
