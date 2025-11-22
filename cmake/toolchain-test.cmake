# ====================================
# Test Toolchain Configuration
# ====================================
# Uses system GCC/G++ for unit testing on x86_64

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Use system GCC/G++
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# Compiler flags for testing
set(CMAKE_C_FLAGS_INIT "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_INIT "-Wall -Wextra")

# Test-specific definitions
add_compile_definitions(
    PC_PLATFORM
    UNIT_TEST_BUILD
    HARDWARE_SIMULATION
)

message(STATUS "Using Test Toolchain:")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Processor: ${CMAKE_SYSTEM_PROCESSOR}")
