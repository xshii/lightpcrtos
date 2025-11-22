# LightPCRTOS 快速开始

## 🚀 快速运行

### 1. 运行 Linux 模拟应用（最快）

```bash
# 构建
./scripts/build_linux.sh

# 运行
./build_linux/src/lightpcrtos

# 优雅退出: Ctrl+C
```

### 2. 运行单元测试

```bash
make test
```

### 3. 使用 Docker（推荐用于嵌入式编译）

```bash
# 构建 Docker 镜像
make docker-build

# 运行 Linux 应用
make docker-run

# 进入开发环境
make docker-shell

# 在 Docker 内编译嵌入式固件
./scripts/build_embedded.sh
```

## 📂 项目结构

```
lightpcrtos/
├── src/
│   ├── main.c              # 嵌入式入口点（RTOS任务）
│   ├── main_linux.c        # Linux入口点（pthread线程）
│   ├── business/           # 业务逻辑
│   │   ├── system_init.c   # 系统初始化
│   │   ├── sensor/         # 传感器管理
│   │   └── control/        # 电机控制
│   └── hal/                # 硬件抽象层接口
├── test/
│   ├── sim/                # 硬件模拟器
│   ├── framework/          # 测试框架
│   └── testcases/          # 87个单元测试
├── cmake/
│   ├── toolchain-test.cmake      # 测试工具链 (GCC)
│   └── toolchain-production.cmake # 生产工具链 (ARM GCC)
├── scripts/
│   ├── build_test.sh       # 构建单元测试
│   ├── build_linux.sh      # 构建Linux应用
│   └── build_embedded.sh   # 构建嵌入式固件
├── Dockerfile              # 开发环境
├── docker-compose.yml      # Docker配置
└── Makefile                # 便捷命令
```

## 🛠️ 构建目标

| 目标 | 工具链 | 编译器 | 用途 |
|------|--------|--------|------|
| **测试** | Test | GCC/G++ | 单元测试 (x86_64) |
| **Linux应用** | Test | GCC/G++ | 模拟运行 (pthread) |
| **嵌入式固件** | Production | ARM GCC | 真实硬件 (Cortex-M4) |

## 🎯 使用场景

### 开发和调试
```bash
# 快速验证功能
make linux
./build_linux/src/lightpcrtos
```

### 持续集成
```bash
# 运行所有测试
make test
```

### 生产部署
```bash
# Docker环境编译
make docker-shell
./scripts/build_embedded.sh

# 输出文件:
# - lightpcrtos.elf  (ELF可执行文件)
# - lightpcrtos.bin  (二进制固件)
# - lightpcrtos.hex  (HEX格式)
```

## 💡 Makefile 命令

```bash
make help           # 显示所有命令
make docker-build   # 构建Docker镜像
make docker-run     # 运行Linux应用
make docker-shell   # 进入Docker shell
make test           # 构建并运行测试
make linux          # 构建Linux应用
make embedded       # 构建嵌入式固件
make clean          # 清理所有构建
```

## 🔍 查看详细文档

- **完整构建指南**: [docs/BUILD.md](docs/BUILD.md)
- **测试指南**: [test/README.md](test/README.md)
- **贡献指南**: [CONTRIBUTING.md](CONTRIBUTING.md)

## ⚡ 系统要求

### 最小要求（运行测试和Linux应用）
- CMake >= 3.15
- GCC/G++ (系统默认)

### 完整开发环境（含嵌入式编译）
- Docker
- docker-compose

或者本地安装:
- ARM GCC toolchain (arm-none-eabi-gcc)

## 🐛 故障排除

### 问题: 找不到 ARM 工具链
**解决**: 使用 Docker 环境
```bash
make docker-shell
./scripts/build_embedded.sh
```

### 问题: 测试失败
**检查**:
```bash
cd build_test
ctest --verbose --output-on-failure
```

### 问题: Linux 应用段错误
**调试**:
```bash
gdb ./build_linux/src/lightpcrtos
(gdb) run
```

## 📊 输出示例

### Linux 应用运行输出

```
*****************************************
*                                       *
*       LightPCRTOS v1.0.0             *
*       Linux Platform Edition          *
*                                       *
*****************************************

[INIT] Stage 1: Hardware Layer Initialization
[INIT]   ✓ Memory initialized
[INIT]   ✓ DMA initialized
[INIT]   ✓ Timer initialized
[INIT]   ✓ Scheduler initialized

[INIT] Stage 2: Business Layer Initialization
[INIT]   ✓ Sensor Manager initialized
[INIT]   ✓ Motor Control initialized

[INIT] Stage 3: Starting System Scheduler
[INIT]   ✓ Scheduler initialized

========================================
  System Initialization Complete!
========================================

[MAIN] System ready - Press Ctrl+C to shutdown

[SENSOR_THREAD] Reading #10: Temp=25.0°C, Humidity=50%
[MOTOR_THREAD] Motor started at 25% speed
[MONITOR_THREAD] Started
```

## 🎓 下一步

1. 探索源码: 查看 `src/business/system_init.c`
2. 运行测试: `make test`
3. 修改代码并重新编译
4. 提交 PR: 参考 [CONTRIBUTING.md](CONTRIBUTING.md)

---

**项目主页**: https://github.com/xshii/lightpcrtos
**问题反馈**: https://github.com/xshii/lightpcrtos/issues
