#!/bin/bash

BUILD_DIR="build_test"

if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory not found. Run ./scripts/build_test.sh first"
    exit 1
fi

cd $BUILD_DIR

echo "======================================"
echo "Running all tests..."
echo "======================================"
echo ""

ctest --output-on-failure --verbose

TEST_RESULT=$?

echo ""
echo "======================================"

if [ $TEST_RESULT -eq 0 ]; then
    echo "✓ All tests PASSED"
else
    echo "✗ Some tests FAILED"
fi

echo "======================================"

exit $TEST_RESULT
