# ====================================
# LightPCRTOS Makefile
# ====================================

.PHONY: help docker-build docker-run docker-shell test linux embedded clean

# Default target
help:
	@echo "LightPCRTOS Build System"
	@echo ""
	@echo "Docker targets:"
	@echo "  docker-build    - Build Docker image"
	@echo "  docker-run      - Run Linux application in Docker"
	@echo "  docker-shell    - Open shell in Docker container"
	@echo ""
	@echo "Build targets:"
	@echo "  test            - Build and run unit tests"
	@echo "  linux           - Build Linux application"
	@echo "  embedded        - Build embedded target (ARM)"
	@echo ""
	@echo "Utility:"
	@echo "  clean           - Clean all build directories"
	@echo ""

# Docker targets
docker-build:
	@echo "Building Docker image..."
	docker-compose build

docker-run: docker-build
	@echo "Running Linux application in Docker..."
	docker-compose run --rm dev ./scripts/build_linux.sh
	docker-compose run --rm dev ./build_linux/src/lightpcrtos

docker-shell: docker-build
	@echo "Opening Docker shell..."
	docker-compose run --rm dev /bin/bash

# Build targets (can run in Docker or locally)
test:
	@echo "Building and running tests..."
	./scripts/build_test.sh
	./scripts/run_tests.sh

linux:
	@echo "Building Linux application..."
	./scripts/build_linux.sh

embedded:
	@echo "Building embedded target..."
	@if command -v arm-none-eabi-gcc >/dev/null 2>&1; then \
		./scripts/build_embedded.sh; \
	else \
		echo "ERROR: ARM toolchain not found. Run in Docker:"; \
		echo "  make docker-shell"; \
		echo "  ./scripts/build_embedded.sh"; \
		exit 1; \
	fi

# Clean
clean:
	@echo "Cleaning build directories..."
	rm -rf build build_test build_linux build_embedded
	@echo "Clean complete."
