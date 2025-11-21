# ====================================
# Sim Library Unit Tests
# ====================================

# Test memory simulator
add_executable(test_sim_memory
    test_sim_memory.cpp
)

target_link_libraries(test_sim_memory PRIVATE
    GTest::gtest
    GTest::gtest_main
    sim_lib
)

target_compile_options(test_sim_memory PRIVATE -Wall -Wextra -g)

add_test(NAME test_sim_memory COMMAND test_sim_memory)
set_tests_properties(test_sim_memory PROPERTIES
    TIMEOUT 30
    LABELS "sim;memory"
)

# Test DMA simulator
add_executable(test_sim_dma
    test_sim_dma.cpp
)

target_link_libraries(test_sim_dma PRIVATE
    GTest::gtest
    GTest::gtest_main
    sim_lib
)

target_compile_options(test_sim_dma PRIVATE -Wall -Wextra -g)

add_test(NAME test_sim_dma COMMAND test_sim_dma)
set_tests_properties(test_sim_dma PROPERTIES
    TIMEOUT 30
    LABELS "sim;dma"
)

# Test scheduler simulator
add_executable(test_sim_scheduler
    test_sim_scheduler.cpp
)

target_link_libraries(test_sim_scheduler PRIVATE
    GTest::gtest
    GTest::gtest_main
    sim_lib
)

target_compile_options(test_sim_scheduler PRIVATE -Wall -Wextra -g)

add_test(NAME test_sim_scheduler COMMAND test_sim_scheduler)
set_tests_properties(test_sim_scheduler PROPERTIES
    TIMEOUT 30
    LABELS "sim;scheduler"
)

message(STATUS "  Sim library tests configured")
