#!/bin/bash
# ====================================
# Build Linux Application
# ====================================

set -e

BUILD_TYPE="${1:-Debug}"
BUILD_DIR="build_linux"

echo "======================================"
echo "Building Linux application ($BUILD_TYPE)..."
echo "======================================"

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure with test toolchain
cmake .. \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-test.cmake \
    -DBUILD_LINUX_APP=ON \
    -DBUILD_TESTS=OFF \
    -DBUILD_EMBEDDED=OFF

# Build
cmake --build . -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "======================================"
echo "Build complete!"
echo "======================================"
echo ""
echo "Run application:"
echo "  ./build_linux/src/lightpcrtos"
echo ""
