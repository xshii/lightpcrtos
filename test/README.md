# Testing Guide

## Overview

This directory contains the complete testing infrastructure for LightPCRTOS, including hardware simulation libraries and comprehensive test suites.

## Directory Structure

```
test/
├── framework/          # Runtime framework (timer, event, scheduler)
├── sim/               # Hardware simulation library
│   ├── src/          # Simulator implementations
│   └── tests/        # Simulator unit tests
└── testcases/        # Business logic test cases
    ├── sensor/       # Sensor manager tests
    └── control/      # Motor control tests
```

## Running Tests

### Build and run all tests
```bash
./scripts/build_test.sh Debug
./scripts/run_tests.sh
```

### Run specific test suites
```bash
cd build_test
ctest -L sim          # Run simulation library tests only
ctest -L sensor       # Run sensor tests only
ctest -L control      # Run control tests only
```

### Run individual tests
```bash
cd build_test
./test/testcases/sensor/test_sensor      # Sensor manager tests
./test/testcases/control/test_control    # Motor control tests
./test/sim/tests/test_sim_memory        # Memory simulator tests
```

## Debugging with VSCode

The `.vscode/launch.json` file provides several debug configurations:

### Available Debug Configurations

1. **Debug Sensor Tests** - Debug all sensor manager tests
2. **Debug Control Tests** - Debug all motor control tests
3. **Debug Sensor Tests (Filter)** - Debug specific sensor tests using filters
4. **Debug Current Test** - Debug the currently active test
5. **Debug Sim Memory Tests** - Debug memory simulator tests
6. **Debug Sim DMA Tests** - Debug DMA simulator tests
7. **Debug Sim Scheduler Tests** - Debug scheduler simulator tests
8. **Debug Sim Timer Tests** - Debug timer simulator tests

### Using Test Filters

To debug a specific test case, use the filter configuration:

```json
{
    "name": "Debug Sensor Tests (Filter)",
    "args": ["--gtest_filter=*ReadSensorData"]
}
```

Or run from command line:
```bash
./test/testcases/sensor/test_sensor --gtest_filter='*ReadSensorData'
```

Filter patterns:
- `*ReadSensorData` - Run tests matching this pattern
- `SensorManagerTest.*` - Run all tests in SensorManagerTest suite
- `*Init*` - Run all tests with "Init" in the name
- `-*Error*` - Exclude tests with "Error" in the name

## Test Coverage

### Simulation Library Tests (53 tests)
- **Memory Simulator** (14 tests): Pool configuration, allocation, cache operations
- **DMA Simulator** (13 tests): Channel management, transfers, callbacks
- **Scheduler Simulator** (13 tests): Task creation, priorities, lifecycle
- **Timer Simulator** (13 tests): Timer creation, callbacks, one-shot/periodic modes, delays

### Business Logic Tests (22 tests)
- **Sensor Manager** (12 tests): Initialization, reading, sampling, edge cases
- **Motor Control** (10 tests): Modes, speed control, lifecycle, error handling

## Writing New Tests

### Test File Template

```cpp
#include <gtest/gtest.h>

extern "C" {
#include "your_module.h"
#include "sim_memory.h"  // If using memory
}

class YourModuleTest : public ::testing::Test {
protected:
    void SetUp() override {
        SIM_MEMORY_SimulatorInit();
        SIM_MEMORY_ConfigurePool(POOL_NAME_SRAM,
                                 (void*)0x20000000,
                                 1024 * 1024);
    }

    void TearDown() override {
        YOUR_MODULE_Deinit();  // Clean up module state
        SIM_MEMORY_SimulatorReset();
    }
};

TEST_F(YourModuleTest, BasicFunctionality) {
    ASSERT_EQ(HAL_OK, YOUR_MODULE_Init());
    // Add test logic here
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

### Best Practices

1. **Always clean up in TearDown**: Call module deinit functions to ensure test isolation
2. **Test edge cases**: Null pointers, uninitialized access, boundary conditions
3. **Use descriptive test names**: `TEST_F(ModuleTest, DoesSpecificThingCorrectly)`
4. **Configure simulators in SetUp**: Initialize memory pools, DMA instances before tests
5. **Verify state changes**: Don't just check return codes, verify the actual state

## Adding Tests to CMake

To add a new test module:

1. Create test directory: `test/testcases/your_module/`
2. Add CMakeLists.txt:
```cmake
add_module_test(your_module
    test_your_module.cpp
)
```
3. Include in parent CMakeLists.txt:
```cmake
add_subdirectory(your_module)
```

The `add_module_test` function automatically:
- Links with business, sim, and framework libraries
- Registers the test with CTest
- Sets up proper include paths

## Continuous Integration

All tests run automatically on:
- Every push and pull request (CI workflow)
- Daily nightly builds with extended testing
- Release builds for version tags

See `.github/workflows/` for CI configuration.
