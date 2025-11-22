# ====================================
# Test Build Configuration
# ====================================

message(STATUS "Configuring test build...")

# Find Google Test
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(STATUS "Google Test not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Build framework library
add_subdirectory(framework)

# Build simulation library
add_subdirectory(sim)

# Build business code for testing
add_library(business_for_test STATIC
    ../src/business/sensor/sensor_manager.c
    ../src/business/control/motor_control.c
    ../src/business/system_init.c
)

target_include_directories(business_for_test PUBLIC
    ../src/hal
    ../src/business
    ../src/business/sensor
    ../src/business/control
)

target_link_libraries(business_for_test PUBLIC
    sim_lib
    framework_lib
)

target_compile_definitions(business_for_test PRIVATE
    UNIT_TEST_BUILD
    PC_PLATFORM
)

# Add coverage flags if enabled
if(ENABLE_COVERAGE)
    target_compile_options(business_for_test PRIVATE --coverage)
    target_link_options(business_for_test PRIVATE --coverage)
endif()

# Build test cases
add_subdirectory(testcases)

message(STATUS "Test build configured")
