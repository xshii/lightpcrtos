# ====================================
# Test Cases
# ====================================

# Helper function to create test executables
function(add_module_test module_name)
    set(test_target "test_${module_name}")

    add_executable(${test_target} ${ARGN})

    target_link_libraries(${test_target} PRIVATE
        GTest::gtest
        GTest::gtest_main
        business_for_test
        sim_lib
        framework_lib
    )

    target_compile_options(${test_target} PRIVATE
        -Wall -Wextra -g
    )

    # Code coverage
    if(ENABLE_COVERAGE)
        target_compile_options(${test_target} PRIVATE --coverage)
        target_link_options(${test_target} PRIVATE --coverage)
    endif()

    # Add to CTest
    add_test(NAME ${test_target} COMMAND ${test_target})

    set_tests_properties(${test_target} PROPERTIES
        TIMEOUT 60
        LABELS ${module_name}
    )

    message(STATUS "  Added test: ${test_target}")
endfunction()

# Add test modules
add_subdirectory(sensor)
add_subdirectory(control)
