# ====================================
# Business Modules
# ====================================

# Collect all business source files
set(BUSINESS_SOURCES "")

# Network module
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/network)
    file(GLOB NETWORK_SOURCES network/*.c)
    list(APPEND BUSINESS_SOURCES ${NETWORK_SOURCES})
endif()

# Sensor module
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/sensor)
    file(GLOB SENSOR_SOURCES sensor/*.c)
    list(APPEND BUSINESS_SOURCES ${SENSOR_SOURCES})
endif()

# Control module
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/control)
    file(GLOB CONTROL_SOURCES control/*.c)
    list(APPEND BUSINESS_SOURCES ${CONTROL_SOURCES})
endif()

# Storage module
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/storage)
    file(GLOB STORAGE_SOURCES storage/*.c)
    list(APPEND BUSINESS_SOURCES ${STORAGE_SOURCES})
endif()

# Create business static library
add_library(business STATIC ${BUSINESS_SOURCES})

# Set library properties
set_target_properties(business PROPERTIES
    OUTPUT_NAME "business"
    POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(business PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/sensor
    ${CMAKE_CURRENT_SOURCE_DIR}/control
    ${CMAKE_CURRENT_SOURCE_DIR}/storage
    ${CMAKE_SOURCE_DIR}/src/hal
)

# Compile options
target_compile_options(business PRIVATE
    -Wall
    -Wextra
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O2>
)

message(STATUS "Business library configured with ${CMAKE_BUILD_TYPE} build")
