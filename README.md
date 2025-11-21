# LightPCRTOS - DSP Business Software Testing Framework

[![CI](https://github.com/YOUR_USERNAME/lightpcrtos/actions/workflows/ci.yml/badge.svg)](https://github.com/YOUR_USERNAME/lightpcrtos/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/YOUR_USERNAME/lightpcrtos/branch/main/graph/badge.svg)](https://codecov.io/gh/YOUR_USERNAME/lightpcrtos)

A comprehensive testing framework for DSP embedded business software, enabling PC-based unit testing with hardware simulation.

## Features

- **Business-only codebase**: No driver implementation in src/, purely business logic
- **Hardware simulation**: Complete HAL implementation via simulation library (test/sim/)
- **Multiple static libraries**:
  - `business.a` - Business logic modules
  - `sim_lib.a` - Hardware simulation (Scheduler, DMA, Memory)
  - `framework_lib.a` - Runtime framework (Timer, Event, Scheduler)
- **Google Test**: Modern C++ testing framework
- **Modular design**: Each business module tested independently

## Project Structure

```
lightpcrtos/
├── src/                    # Business code (compiled as static library)
│   ├── business/          # Business modules
│   │   ├── sensor/       # Sensor management
│   │   └── control/      # Motor control
│   └── hal/              # HAL interface definitions (headers only)
│
└── test/                  # Testing infrastructure
    ├── framework/         # Runtime framework (timer, event, scheduler)
    ├── sim/              # Hardware simulation library
    │   ├── src/
    │   │   ├── sim_scheduler.c  # Implements HAL_SCHEDULER_*
    │   │   ├── sim_dma.c        # Implements HAL_DMA_*
    │   │   └── sim_memory.c     # Implements HAL_MEMORY_*
    │   └── include/
    └── testcases/        # Test cases using Google Test
        ├── sensor/
        └── control/
```

## HAL Interfaces

### Memory (hal_memory.h)
- Named pool allocation: `HAL_MEMORY_AllocBuffer(POOL_NAME_L1, size, &buffer)`
- Cache operations: `HAL_MEMORY_FlushBuffer()`, `HAL_MEMORY_InvalidateBuffer()`
- Address translation: `HAL_MEMORY_GetPhysAddr()`
- **Sim configuration**: `SIM_MEMORY_ConfigurePool("L1", baseAddr, size)`

### DMA (hal_dma.h)
- Multi-instance support: `HAL_DMA_Init(dmaId, config)`
- Multiple DMA types: Standard, 2D, Scatter-Gather, Stream
- Channel management: `HAL_DMA_RequestChannel(dmaId, direction, priority, &channel)`
- Event callbacks: `HAL_DMA_RegisterCallback(channel, callback, userData)`

### Scheduler (hal_scheduler.h)
- Task management: `HAL_SCHEDULER_CreateTask(func, args, priority, &handle)`
- Task control: Suspend, Resume, Yield
- State queries: `HAL_SCHEDULER_GetTaskState(handle)`

## Building

### Build Tests

```bash
./scripts/build_test.sh Debug
```

### Run Tests

Run all tests:
```bash
./scripts/run_tests.sh
```

Or manually:
```bash
cd build_test
ctest --output-on-failure
```

Run only sim library tests:
```bash
cd build_test
ctest -L sim --output-on-failure
```

Run specific sim test:
```bash
cd build_test
./test/sim/tests/test_sim_memory
./test/sim/tests/test_sim_dma
./test/sim/tests/test_sim_scheduler
```

## Writing Tests

Example test case using Google Test:

```cpp
#include <gtest/gtest.h>

extern "C" {
#include "sensor_manager.h"
#include "sim_memory.h"
}

class SensorTest : public ::testing::Test {
protected:
    void SetUp() override {
        SIM_MEMORY_SimulatorInit();
        SIM_MEMORY_ConfigurePool(POOL_NAME_SRAM,
                                 (void*)0x20000000,
                                 1024 * 1024);
    }
};

TEST_F(SensorTest, ReadData) {
    SENSOR_MANAGER_Init();

    SensorData data;
    int ret = SENSOR_MANAGER_Read(&data);

    EXPECT_EQ(HAL_OK, ret);
    EXPECT_TRUE(data.valid);
}
```

## Naming Conventions

- **Public APIs**: `MODULE_NAME_FunctionName()` (all caps + underscore)
- **Private functions**: `ModuleNameFunctionName()` (CamelCase, static)
- **Types/Structs**: `TypeName` (CamelCase)
- **Variables**: `variableName` (camelCase)
- **Global variables**: `g_variableName` (g_ prefix + camelCase)

## Key Design Points

### 1. Hardware Resources Binding
- Sim library supports resource pooling and binding to logical resources
- DMA instances can be assigned to specific memory pools
- Configurable address ranges for each cache/pool

### 2. Opaque Configuration
- Platform-specific config hidden behind `void*` types
- DMA: `typedef void* DmaConfig`
- Allows different platforms to have different config structures

### 3. Simulation Control
- Test-specific APIs (prefixed with `SIM_*`) for:
  - Configuring address ranges
  - Injecting events
  - Querying statistics

## Dependencies

- CMake >= 3.15
- C99 compiler (GCC/Clang)
- C++11 compiler (for Google Test)
- Google Test (auto-downloaded if not found)

## CI/CD

This project uses GitHub Actions for continuous integration:

- **CI Workflow**: Runs on every push and pull request
  - Ubuntu + Debug build only (fast feedback)
  - Runs all tests (sim + business)
  - Generates code coverage reports
  - Static analysis with cppcheck
  - Format checking with clang-format

- **Release Workflow**: Triggered on version tags (v*)
  - Builds release artifacts for Linux and macOS
  - Creates GitHub releases with binaries

- **Nightly Tests**: Daily comprehensive testing
  - Memory leak detection with Valgrind
  - Stress testing (100 iterations)
  - Extended coverage reports

### Running Locally

Format code:
```bash
find src test -name "*.c" -o -name "*.h" -o -name "*.cpp" | xargs clang-format -i
```

Static analysis:
```bash
cppcheck --enable=all src/business/ test/sim/src/
```

## License

[Your License Here]

## Contributing

[Contributing guidelines]
