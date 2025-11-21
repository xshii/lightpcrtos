#!/bin/bash

set -e

BUILD_DIR="build_test"
BUILD_TYPE=${1:-Debug}

echo "======================================"
echo "Building tests ($BUILD_TYPE)..."
echo "======================================"

# Create build directory
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DBUILD_TESTS=ON \
    -DBUILD_EMBEDDED=OFF \
    -DENABLE_COVERAGE=OFF

# Build
make -j$(nproc 2>/dev/null || echo 4)

echo ""
echo "======================================"
echo "Build complete!"
echo "======================================"
echo ""
echo "Run tests with:"
echo "  cd $BUILD_DIR && ctest --output-on-failure"
echo "  or"
echo "  ./scripts/run_tests.sh"
