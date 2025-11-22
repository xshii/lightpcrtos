# ====================================
# LightPCRTOS Development Environment
# ====================================

FROM ubuntu:22.04

# Prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install basic development tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    curl \
    python3 \
    python3-pip \
    ninja-build \
    clang-format \
    cppcheck \
    lcov \
    gdb \
    vim \
    && rm -rf /var/lib/apt/lists/*

# Install ARM GCC toolchain for production builds
RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    binutils-arm-none-eabi \
    libnewlib-arm-none-eabi \
    && rm -rf /var/lib/apt/lists/*

# Set up work directory
WORKDIR /workspace

# Create toolchain files directory
RUN mkdir -p /opt/toolchains

# Set environment variables
ENV PATH="/usr/bin:${PATH}"
ENV CC_FOR_TEST=gcc
ENV CXX_FOR_TEST=g++
ENV CC_FOR_PRODUCTION=arm-none-eabi-gcc
ENV CXX_FOR_PRODUCTION=arm-none-eabi-g++

# Print toolchain versions
RUN echo "=== Test Toolchain ===" && \
    gcc --version && \
    g++ --version && \
    echo "\n=== Production Toolchain ===" && \
    arm-none-eabi-gcc --version && \
    arm-none-eabi-g++ --version

# Default command
CMD ["/bin/bash"]
