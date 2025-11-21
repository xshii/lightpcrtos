# Contributing to LightPCRTOS

## Development Workflow

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Run tests locally
5. Commit your changes
6. Push to your fork
7. Open a Pull Request

## Code Style

### Naming Conventions

- **Public APIs**: `MODULE_NAME_FunctionName()` (全大写+下划线)
  ```c
  int HAL_MEMORY_AllocBuffer(PoolName poolName, size_t size, MemoryBuffer* buffer);
  ```

- **Private functions**: `ModuleNameFunctionName()` (大驼峰, static)
  ```c
  static int SensorManagerAllocateDataBuffer(void);
  ```

- **Types/Structs**: `TypeName` (大驼峰)
  ```c
  typedef struct {
      float temperature;
  } SensorData;
  ```

- **Variables**: `variableName` (小驼峰)
  ```c
  int bufferSize = 1024;
  ```

- **Global variables**: `g_variableName`
  ```c
  static SensorData g_currentData;
  ```

### Code Formatting

Use clang-format before committing:
```bash
find src test -name "*.c" -o -name "*.h" -o -name "*.cpp" | xargs clang-format -i
```

## Testing Requirements

### All Changes Must Include Tests

1. **Business code changes**: Add tests in `test/testcases/`
2. **Sim library changes**: Add tests in `test/sim/tests/`

### Running Tests Locally

```bash
# Build and run all tests
./scripts/build_test.sh Debug
./scripts/run_tests.sh

# Run specific test suite
cd build_test
ctest -L sim          # Sim library tests
ctest -L unit         # Business logic tests
```

### Test Coverage

- Aim for >80% code coverage
- All public APIs must be tested
- Edge cases and error paths must be covered

## Pull Request Guidelines

### PR Title Format

- `feat: Add new feature`
- `fix: Fix bug in module`
- `test: Add tests for feature`
- `docs: Update documentation`
- `refactor: Refactor code`

### PR Description

Include:
- What changes were made
- Why the changes were necessary
- Test results (paste ctest output)
- Any breaking changes

### CI Checks

All PRs must pass:
- ✅ Build on Ubuntu and macOS
- ✅ All tests pass
- ✅ Code coverage maintained
- ✅ Static analysis clean
- ✅ Code formatting check

## Adding New HAL Interfaces

1. Define interface in `src/hal/hal_newmodule.h`
2. Implement simulation in `test/sim/src/sim_newmodule.c`
3. Add sim tests in `test/sim/tests/test_sim_newmodule.cpp`
4. Update documentation

Example:
```c
// src/hal/hal_uart.h
int HAL_UART_Init(UartId uartId, UartConfig config);
int HAL_UART_Write(UartId uartId, const uint8_t* data, size_t size);
int HAL_UART_Read(UartId uartId, uint8_t* data, size_t size);
```

## Adding New Business Modules

1. Create module in `src/business/newmodule/`
2. Add CMakeLists.txt
3. Write tests in `test/testcases/newmodule/`
4. Update main business CMakeLists.txt

## Commit Messages

Follow conventional commits:

```
<type>(<scope>): <subject>

<body>

<footer>
```

Example:
```
feat(sim): Add UART simulation support

Implemented full UART simulation with:
- TX/RX buffer management
- Baud rate simulation
- Error injection capabilities

Closes #123
```

## Questions?

Open an issue or discussion on GitHub.
