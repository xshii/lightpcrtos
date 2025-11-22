# ====================================
# Production Toolchain Configuration
# ====================================
# ARM Cortex-M4 bare-metal cross-compilation

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Specify the cross compiler
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)

# Target processor flags (Cortex-M4 with FPU)
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# Common flags
set(COMMON_FLAGS "${CPU_FLAGS} -ffunction-sections -fdata-sections -Wall -Wextra")

# C flags
set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "-Og -g3")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

# C++ flags
set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -fno-exceptions -fno-rtti")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CPU_FLAGS} -specs=nano.specs -specs=nosys.specs -Wl,--gc-sections")

# Production-specific definitions
add_compile_definitions(
    EMBEDDED_TARGET
    ARM_CORTEX_M4
    USE_HAL_DRIVER
)

# Don't search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

message(STATUS "Using Production Toolchain:")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  Target CPU: Cortex-M4 with FPU")
message(STATUS "  System: ${CMAKE_SYSTEM_NAME}")
