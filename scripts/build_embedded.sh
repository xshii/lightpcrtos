#!/bin/bash
# ====================================
# Build Embedded Target (ARM)
# ====================================

set -e

BUILD_TYPE="${1:-Release}"
BUILD_DIR="build_embedded"

echo "======================================"
echo "Building embedded target ($BUILD_TYPE)..."
echo "======================================"

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure with production toolchain
cmake .. \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-production.cmake \
    -DBUILD_EMBEDDED=ON \
    -DBUILD_TESTS=OFF \
    -DBUILD_LINUX_APP=OFF

# Build
cmake --build . -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "======================================"
echo "Build complete!"
echo "======================================"
echo ""
echo "Output files:"
echo "  ./build_embedded/src/lightpcrtos.elf"
echo "  ./build_embedded/src/lightpcrtos.bin"
echo "  ./build_embedded/src/lightpcrtos.hex"
echo ""
