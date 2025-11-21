# ====================================
# Business Runtime Framework Library
# ====================================

# Framework sources (if exist)
set(FRAMEWORK_SOURCES "")

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/framework_timer.c)
    list(APPEND FRAMEWORK_SOURCES src/framework_timer.c)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/framework_event.c)
    list(APPEND FRAMEWORK_SOURCES src/framework_event.c)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/framework_scheduler.c)
    list(APPEND FRAMEWORK_SOURCES src/framework_scheduler.c)
endif()

# Create framework library (even if empty for now)
add_library(framework_lib STATIC ${FRAMEWORK_SOURCES})

target_include_directories(framework_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(framework_lib PROPERTIES
    OUTPUT_NAME "framework"
)

# Add coverage flags if enabled
if(ENABLE_COVERAGE)
    target_compile_options(framework_lib PRIVATE --coverage)
    target_link_options(framework_lib PRIVATE --coverage)
endif()

message(STATUS "Framework library configured")
