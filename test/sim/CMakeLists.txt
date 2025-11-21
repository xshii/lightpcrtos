# ====================================
# Hardware Simulation Library
# ====================================

message(STATUS "Building simulation library...")

# Simulation sources
set(SIM_SOURCES
    src/sim_scheduler.c
    src/sim_dma.c
    src/sim_memory.c
    src/sim_timer.c
)

# Create simulation library
add_library(sim_lib STATIC ${SIM_SOURCES})

target_include_directories(sim_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/hal
)

target_compile_definitions(sim_lib PUBLIC
    HARDWARE_SIMULATION
    SIM_LIB_VERSION_MAJOR=1
    SIM_LIB_VERSION_MINOR=0
)

target_compile_options(sim_lib PRIVATE
    -Wall -Wextra
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O2>
)

set_target_properties(sim_lib PROPERTIES
    OUTPUT_NAME "hardware_sim"
)

# Add sim library tests
add_subdirectory(tests)

message(STATUS "Simulation library configured")
