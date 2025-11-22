# LightPCRTOS 构建指南

本文档介绍如何构建和运行 LightPCRTOS 的不同目标平台。

## 目录

- [构建环境](#构建环境)
- [构建目标](#构建目标)
- [Docker 使用](#docker-使用)
- [本地构建](#本地构建)
- [工具链说明](#工具链说明)

## 构建环境

### Docker 环境（推荐）

使用 Docker 可以获得一致的构建环境，包含所有必要的工具链。

```bash
# 构建 Docker 镜像
make docker-build

# 或使用 docker-compose
docker-compose build
```

### 本地环境要求

**测试工具链（必需）：**
- GCC/G++ (系统默认)
- CMake >= 3.15
- Google Test (自动下载)

**生产工具链（可选）：**
- ARM GCC toolchain (arm-none-eabi-gcc)
- 仅在构建嵌入式目标时需要

## 构建目标

LightPCRTOS 支持三种构建目标：

### 1. 单元测试（Test Toolchain）

使用系统 GCC/G++ 编译和运行单元测试。

```bash
# 本地构建
make test

# 或手动
./scripts/build_test.sh
./scripts/run_tests.sh

# Docker 中构建
docker-compose run --rm dev make test
```

**输出：**
- `build_test/` - 测试可执行文件
- 87 个单元测试，100% 通过率

### 2. Linux 应用（Test Toolchain）

使用系统 GCC/G++ 编译 Linux 模拟应用，使用硬件模拟器。

```bash
# 本地构建
make linux

# 或手动
./scripts/build_linux.sh

# 运行
./build_linux/src/lightpcrtos

# Docker 中运行
make docker-run
```

**特性：**
- 多线程模拟（pthread）
- 硬件模拟器（Memory, DMA, Timer, Scheduler）
- 三个应用线程：传感器、电机控制、系统监控
- Ctrl+C 优雅退出

### 3. 嵌入式目标（Production Toolchain）

使用 ARM 交叉编译器编译嵌入式固件（Cortex-M4）。

```bash
# Docker 中构建（推荐）
make docker-shell
./scripts/build_embedded.sh

# 本地构建（需要 ARM 工具链）
make embedded
```

**输出：**
- `build_embedded/src/lightpcrtos.elf` - ELF 可执行文件
- `build_embedded/src/lightpcrtos.bin` - 二进制固件
- `build_embedded/src/lightpcrtos.hex` - Intel HEX 格式

## Docker 使用

### 快速开始

```bash
# 1. 构建 Docker 镜像
make docker-build

# 2. 运行 Linux 应用
make docker-run

# 3. 进入开发环境
make docker-shell
```

### Docker 命令详解

```bash
# 构建镜像
docker-compose build

# 运行单元测试
docker-compose run --rm dev ./scripts/build_test.sh
docker-compose run --rm dev ./scripts/run_tests.sh

# 构建 Linux 应用
docker-compose run --rm dev ./scripts/build_linux.sh

# 运行 Linux 应用
docker-compose run --rm dev ./build_linux/src/lightpcrtos

# 构建嵌入式目标
docker-compose run --rm dev ./scripts/build_embedded.sh

# 交互式 shell
docker-compose run --rm dev /bin/bash
```

### Docker 卷挂载

Docker Compose 配置了以下卷：
- `.:/workspace` - 源代码挂载
- `build-cache:/workspace/build` - 构建缓存
- `build-test-cache:/workspace/build_test` - 测试构建缓存

## 本地构建

### 安装依赖

**Ubuntu/Debian:**
```bash
# 基础工具
sudo apt-get install build-essential cmake git

# ARM 工具链（可选）
sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi
```

**macOS:**
```bash
# 基础工具
brew install cmake

# ARM 工具链（可选）
brew install arm-none-eabi-gcc
```

### 手动构建

**测试：**
```bash
mkdir -p build_test
cd build_test
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-test.cmake \
    -DBUILD_TESTS=ON \
    -DBUILD_LINUX_APP=OFF \
    -DBUILD_EMBEDDED=OFF
cmake --build . -j$(nproc)
ctest --output-on-failure
```

**Linux 应用：**
```bash
mkdir -p build_linux
cd build_linux
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-test.cmake \
    -DBUILD_LINUX_APP=ON \
    -DBUILD_TESTS=OFF \
    -DBUILD_EMBEDDED=OFF
cmake --build . -j$(nproc)
./src/lightpcrtos
```

**嵌入式目标：**
```bash
mkdir -p build_embedded
cd build_embedded
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-production.cmake \
    -DBUILD_EMBEDDED=ON \
    -DBUILD_TESTS=OFF \
    -DBUILD_LINUX_APP=OFF \
    -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

## 工具链说明

### Test Toolchain (toolchain-test.cmake)

**用途：**
- 单元测试
- Linux 模拟应用

**编译器：**
- C: `gcc`
- C++: `g++`

**目标平台：**
- Linux x86_64

**编译选项：**
- `-DUNIT_TEST_BUILD`
- `-DPC_PLATFORM`
- `-DHARDWARE_SIMULATION`

### Production Toolchain (toolchain-production.cmake)

**用途：**
- 嵌入式固件

**编译器：**
- C: `arm-none-eabi-gcc`
- C++: `arm-none-eabi-g++`

**目标平台：**
- ARM Cortex-M4
- FPU: FPv4-SP-D16 (硬浮点)

**编译选项：**
- `-mcpu=cortex-m4`
- `-mthumb`
- `-mfpu=fpv4-sp-d16`
- `-mfloat-abi=hard`
- `-DEMBEDDED_TARGET`
- `-DARM_CORTEX_M4`

**链接选项：**
- `--specs=nano.specs` - Newlib nano（减小体积）
- `--specs=nosys.specs` - 无系统调用
- `--gc-sections` - 删除未使用的段

## 清理

```bash
# 清理所有构建目录
make clean

# 或手动
rm -rf build build_test build_linux build_embedded
```

## 常见问题

### Q: Docker 构建很慢？
A: 首次构建会下载依赖，后续构建会使用缓存。使用 `docker-compose build --no-cache` 强制重新构建。

### Q: 本地没有 ARM 工具链怎么办？
A: 使用 Docker 环境，或者只构建测试和 Linux 应用。

### Q: 如何指定构建类型？
A: 传递参数给构建脚本：
```bash
./scripts/build_linux.sh Release
./scripts/build_embedded.sh Debug
```

### Q: 如何添加自定义编译选项？
A: 编辑对应的 toolchain 文件：
- `cmake/toolchain-test.cmake`
- `cmake/toolchain-production.cmake`
