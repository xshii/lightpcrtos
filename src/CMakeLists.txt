# ====================================
# Business Logic Static Library
# ====================================

message(STATUS "Configuring business library...")

# Include HAL headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/hal)

# Build business modules
add_subdirectory(business)

# Export include directories for external use
set(BUSINESS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/business
    PARENT_SCOPE
)
