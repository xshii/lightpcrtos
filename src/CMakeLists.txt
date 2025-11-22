# ====================================
# Source Build Configuration
# ====================================

message(STATUS "Configuring source build...")

# Include HAL headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/hal)

# Collect business source files
set(BUSINESS_SOURCES
    business/sensor/sensor_manager.c
    business/control/motor_control.c
    business/system_init.c
)

# Include directories
set(BUSINESS_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/business
    ${CMAKE_CURRENT_SOURCE_DIR}/business/sensor
    ${CMAKE_CURRENT_SOURCE_DIR}/business/control
)

if(BUILD_LINUX_APP)
    # ====================================
    # Linux Application Build
    # ====================================
    message(STATUS "Building Linux application...")

    # Add simulator libraries (reuse from test)
    add_subdirectory(${CMAKE_SOURCE_DIR}/test/sim ${CMAKE_BINARY_DIR}/sim)
    add_subdirectory(${CMAKE_SOURCE_DIR}/test/framework ${CMAKE_BINARY_DIR}/framework)

    # Create executable
    add_executable(lightpcrtos_linux
        ${BUSINESS_SOURCES}
        main_linux.c
    )

    target_include_directories(lightpcrtos_linux PRIVATE
        ${BUSINESS_INCLUDES}
    )

    target_link_libraries(lightpcrtos_linux PRIVATE
        sim_lib
        framework_lib
        pthread
    )

    target_compile_definitions(lightpcrtos_linux PRIVATE
        LINUX_PLATFORM
        HARDWARE_SIMULATION
    )

    set_target_properties(lightpcrtos_linux PROPERTIES
        OUTPUT_NAME "lightpcrtos"
    )

    message(STATUS "  Linux executable: lightpcrtos")

elseif(BUILD_EMBEDDED)
    # ====================================
    # Embedded Target Build
    # ====================================
    message(STATUS "Building embedded target...")

    # Create executable for embedded
    add_executable(lightpcrtos_embedded
        ${BUSINESS_SOURCES}
        main.c
    )

    target_include_directories(lightpcrtos_embedded PRIVATE
        ${BUSINESS_INCLUDES}
    )

    target_compile_definitions(lightpcrtos_embedded PRIVATE
        EMBEDDED_TARGET
        ARM_CORTEX_M4
    )

    set_target_properties(lightpcrtos_embedded PROPERTIES
        OUTPUT_NAME "lightpcrtos.elf"
        SUFFIX ""
    )

    # Generate binary and hex files
    add_custom_command(TARGET lightpcrtos_embedded POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary lightpcrtos.elf lightpcrtos.bin
        COMMAND ${CMAKE_OBJCOPY} -O ihex lightpcrtos.elf lightpcrtos.hex
        COMMAND ${CMAKE_SIZE} lightpcrtos.elf
        COMMENT "Generating binary and hex files..."
    )

    message(STATUS "  Embedded executable: lightpcrtos.elf")

else()
    # ====================================
    # Library Only (for integration)
    # ====================================
    add_subdirectory(business)
endif()

# Export include directories for external use
set(BUSINESS_INCLUDE_DIRS
    ${BUSINESS_INCLUDES}
    PARENT_SCOPE
)

message(STATUS "Source build configured")
