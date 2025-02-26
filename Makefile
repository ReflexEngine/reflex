# Makefile for ReflexEngine with distribution support
# Usage: make <distro> [clean|none]

# Base directory structure
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = binout

# Default target executable
TARGET_NAME = reflex

# Common compiler flags
BASE_CFLAGS = -Wall -Wextra -I$(SRC_DIR)/headers

# Default distribution (if none specified)
DEFAULT_DISTRO = debian

# Define distribution-specific configurations
# Format: DISTRO_<name>_<variable>

# Debian/Ubuntu
DISTRO_debian_CC = gcc
DISTRO_debian_CFLAGS = $(BASE_CFLAGS) -I/usr/include -I/usr/include/lua5.4
DISTRO_debian_LDFLAGS = -L/usr/lib -llua5.4
DISTRO_debian_TARGET = $(BIN_DIR)/$(TARGET_NAME)

# Arch Linux
DISTRO_arch_CC = gcc
DISTRO_arch_CFLAGS = $(BASE_CFLAGS) -I/usr/include -I/usr/include/lua
DISTRO_arch_LDFLAGS = -L/usr/lib -llua
DISTRO_arch_TARGET = $(BIN_DIR)/$(TARGET_NAME)

# Alpine Linux
DISTRO_alpine_CC = gcc
DISTRO_alpine_CFLAGS = $(BASE_CFLAGS) -I/usr/include -I/usr/include/lua5.4
DISTRO_alpine_LDFLAGS = -L/usr/lib -llua5.4
DISTRO_alpine_TARGET = $(BIN_DIR)/$(TARGET_NAME)

# macOS
DISTRO_macos_CC = clang
DISTRO_macos_CFLAGS = $(BASE_CFLAGS) -I/usr/local/include -I/usr/local/include/lua
DISTRO_macos_LDFLAGS = -L/usr/local/lib -llua
DISTRO_macos_TARGET = $(BIN_DIR)/$(TARGET_NAME)

# Windows (MinGW)
DISTRO_windows_CC = gcc
DISTRO_windows_CFLAGS = $(BASE_CFLAGS) -I/usr/x86_64-w64-mingw32/include -I/usr/x86_64-w64-mingw32/include/lua
DISTRO_windows_LDFLAGS = -L/usr/x86_64-w64-mingw32/lib -llua
DISTRO_windows_TARGET = $(BIN_DIR)/$(TARGET_NAME).exe

# Find all C source files recursively in src/
SRCS := $(shell find $(SRC_DIR) -type f -name "*.c")

# Extract unique directories containing source files
DIRS := $(sort $(dir $(SRCS)))

# Default target redirects to help
.PHONY: default
default: help

# Print help message
.PHONY: help
help:
	@echo "ReflexEngine Build System"
	@echo "Usage: make <distro> [clean|none]"
	@echo ""
	@echo "Available distributions:"
	@echo "  debian    - Build for Debian/Ubuntu"
	@echo "  arch      - Build for Arch Linux"
	@echo "  alpine    - Build for Alpine Linux"
	@echo "  macos     - Build for macOS"
	@echo "  windows   - Build for Windows (using MinGW)"
	@echo ""
	@echo "Optional second argument:"
	@echo "  clean     - Clean before building"
	@echo "  none      - Just build (default)"
	@echo ""
	@echo "Examples:"
	@echo "  make debian       - Build for Debian"
	@echo "  make arch clean   - Clean and build for Arch"
	@echo "  make windows      - Build for Windows"

# Generic build rule for any distribution
.PHONY: $(DEFAULT_DISTRO) debian arch alpine macos windows
debian arch alpine macos windows:
	@echo "Building for $@ distribution..."
	@$(MAKE) _build_target DISTRO=$@ ACTION=$(word 2, $(MAKECMDGOALS))

# If no second argument is given, default to "none"
.PHONY: _build_target
_build_target:
	@if [ "$(ACTION)" = "clean" ]; then \
		$(MAKE) _do_clean DISTRO=$(DISTRO); \
	fi
	@$(MAKE) _do_build DISTRO=$(DISTRO)

# Internal build rule
.PHONY: _do_build
_do_build:
	@mkdir -p $(BIN_DIR)
	@$(foreach dir,$(patsubst $(SRC_DIR)%,$(BUILD_DIR)/$(DISTRO)%,$(DIRS)), \
		mkdir -p $(dir);)
	
	@echo "Compiling source files..."
	@$(foreach src,$(SRCS), \
		echo "  $(src)"; \
		$(DISTRO_$(DISTRO)_CC) $(DISTRO_$(DISTRO)_CFLAGS) -g -c $(src) \
			-o $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/$(DISTRO)/%.o,$(src)); \
	)
	
	@echo "Linking target: $(DISTRO_$(DISTRO)_TARGET)"
	@$(DISTRO_$(DISTRO)_CC) \
		$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/$(DISTRO)/%.o,$(SRCS)) \
		$(DISTRO_$(DISTRO)_LDFLAGS) -o $(DISTRO_$(DISTRO)_TARGET)
	
	@echo "Build complete for $(DISTRO)!"

# Internal clean rule
.PHONY: _do_clean
_do_clean:
	@echo "Cleaning build files for $(DISTRO)..."
	@rm -rf $(BUILD_DIR)/$(DISTRO)
	@rm -f $(DISTRO_$(DISTRO)_TARGET)

# Clean all distributions
.PHONY: clean_all
clean_all:
	@echo "Cleaning all build files..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(BIN_DIR)

# Run the Lua runtime (for the default distribution)
.PHONY: run
run: $(DEFAULT_DISTRO)
	@echo "Running $(DISTRO_$(DEFAULT_DISTRO)_TARGET)..."
	@$(DISTRO_$(DEFAULT_DISTRO)_TARGET)

# Handle any other arguments by doing nothing
.PHONY: none clean
none clean:
	@: