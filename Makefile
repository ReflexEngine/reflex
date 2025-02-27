# ReflexEngine Makefile with distribution support
# Usage: make <distro> [clean|none]

# Include generated config if exists
-include config.mk

# Base directory structure
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = binout
INCLUDE_DIR = include

# Default target executable
TARGET_NAME = reflex

# Common compiler flags
BASE_CFLAGS = -Wall -Wextra -I$(SRC_DIR)/headers -I$(INCLUDE_DIR)
BASE_LDFLAGS = -L./$(INCLUDE_DIR)

# If config.mk defined a SELECTED_DISTRO, use it as default
ifdef SELECTED_DISTRO
  DEFAULT_DISTRO = $(SELECTED_DISTRO)
else
  DEFAULT_DISTRO = debian
endif

# Define distribution-specific configurations
# Format: DISTRO_<name>_<variable>

# Debian/Ubuntu
DISTRO_debian_CC = gcc
DISTRO_debian_CFLAGS = $(BASE_CFLAGS) -I/usr/include -I/usr/include/lua5.4
DISTRO_debian_LDFLAGS = $(BASE_LDFLAGS) -L/usr/lib -llua5.4 -luv
DISTRO_debian_TARGET = $(BIN_DIR)/$(TARGET_NAME)-deb

# Arch Linux
DISTRO_arch_CC = gcc
DISTRO_arch_CFLAGS = $(BASE_CFLAGS) -I/usr/include -I/usr/include/lua5.4
DISTRO_arch_LDFLAGS = $(BASE_LDFLAGS) -L/usr/lib -llua5.4 -luv
DISTRO_arch_TARGET = $(BIN_DIR)/$(TARGET_NAME)-arch

# Alpine Linux
DISTRO_alpine_CC = gcc
DISTRO_alpine_CFLAGS = $(BASE_CFLAGS) -I/usr/include -I/usr/include/lua5.4
DISTRO_alpine_LDFLAGS = $(BASE_LDFLAGS) -L/usr/lib -llua5.4 -luv
DISTRO_alpine_TARGET = $(BIN_DIR)/$(TARGET_NAME)-alpine

# macOS (Intel or Apple Silicon)
DISTRO_macos_CC = gcc
DISTRO_macos_CFLAGS = $(BASE_CFLAGS) $(shell \
	if [ "`uname -m`" = "arm64" ]; then \
		echo "-I/opt/homebrew/include -I/usr/include -I/opt/homebrew/include/lua"; \
	else \
		echo "-I/usr/local/include -I/usr/include -I/usr/local/include/lua"; \
	fi)
DISTRO_macos_LDFLAGS = $(BASE_LDFLAGS) $(shell \
	if [ "`uname -m`" = "arm64" ]; then \
		echo "-L/opt/homebrew/lib -L/usr/lib -llua -luv"; \
	else \
		echo "-L/usr/local/lib -L/usr/lib -llua -luv"; \
	fi)
DISTRO_macos_TARGET = $(BIN_DIR)/$(TARGET_NAME)-macos

# Windows (MSVC)
DISTRO_windows_CC = gcc
DISTRO_windows_CFLAGS = $(BASE_CFLAGS) -IC:\Program Files\Lua\include
DISTRO_windows_LDFLAGS = $(BASE_LDFLAGS) -LC:\Program Files\Lua\lib -llua -luv
DISTRO_windows_TARGET = $(BIN_DIR)/$(TARGET_NAME)-msvc.exe

# Windows (MinGW)
DISTRO_mingw_CC = gcc
DISTRO_mingw_CFLAGS = $(BASE_CFLAGS) -I/usr/include -I/usr/include/lua
DISTRO_mingw_LDFLAGS = $(BASE_LDFLAGS) -L/usr/lib -llua -luv
DISTRO_mingw_TARGET = $(BIN_DIR)/$(TARGET_NAME)-mingw.exe

# Find all C source files recursively in src/
SRCS := $(shell find $(SRC_DIR) -type f -name "*.c" 2>/dev/null)

# If no sources found, provide a warning
ifeq ($(SRCS),)
$(warning No source files found in $(SRC_DIR). Build will likely fail.)
endif

# Colors and styling for prettier output
RESET := \033[0m
BOLD := \033[1m
DIM := \033[2m
ITALIC := \033[3m
UNDERLINE := \033[4m

# Text colors
BLACK := \033[38;5;0m
WHITE := \033[38;5;15m
BLUE := \033[38;5;39m
GREEN := \033[38;5;78m
YELLOW := \033[38;5;214m
RED := \033[38;5;203m
MAGENTA := \033[38;5;171m
CYAN := \033[38;5;87m
GRAY := \033[38;5;245m
DGRAY := \033[38;5;240m

# Background colors
BG_DGRAY := \033[48;5;236m
BG_BLUE := \033[48;5;39m
BG_GREEN := \033[48;5;78m

# Symbols
ARROW := →
CHECK := ✓
CROSS := ✗
INFO := ℹ
WARNING := ⚠
BULLET := •

# Default target redirects to help
.PHONY: default
default: help

# Configure target
.PHONY: configure
configure:
	@echo
	@echo -e "$(BG_BLUE)$(WHITE)$(BOLD) CONFIGURE $(RESET) Launching configuration script..."
	@chmod +x ./configure.sh
	@./configure.sh

# Print help message
.PHONY: help
help:
	@echo
	@echo -e "$(BLUE)$(BOLD)┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓$(RESET)"
	@echo -e "$(BLUE)$(BOLD)┃$(RESET)$(BOLD) REFLEX ENGINE BUILD SYSTEM                     $(BLUE)$(BOLD)┃$(RESET)"
	@echo -e "$(BLUE)$(BOLD)┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛$(RESET)"
	@echo
	@echo -e "$(YELLOW)$(BOLD)USAGE:$(RESET)"
	@echo -e "  $(CYAN)make$(RESET) $(MAGENTA)<distro>$(RESET) [clean|none]"
	@echo
	@echo -e "$(GREEN)$(BOLD)DISTRIBUTIONS:$(RESET)"
	@echo -e "  $(CYAN)debian$(RESET)    $(DGRAY)$(ARROW)$(RESET) Build for Debian/Ubuntu"
	@echo -e "  $(CYAN)arch$(RESET)      $(DGRAY)$(ARROW)$(RESET) Build for Arch Linux"
	@echo -e "  $(CYAN)alpine$(RESET)    $(DGRAY)$(ARROW)$(RESET) Build for Alpine Linux"
	@echo -e "  $(CYAN)macos$(RESET)     $(DGRAY)$(ARROW)$(RESET) Build for macOS"
	@echo -e "  $(CYAN)windows$(RESET)   $(DGRAY)$(ARROW)$(RESET) Build for Windows (using MSVC)"
	@echo -e "  $(CYAN)mingw$(RESET)     $(DGRAY)$(ARROW)$(RESET) Build for Windows (using MinGW)"
	@echo
	@echo -e "$(GREEN)$(BOLD)SETUP:$(RESET)"
	@echo -e "  $(CYAN)configure$(RESET) $(DGRAY)$(ARROW)$(RESET) Run the configuration script to detect system and install dependencies"
	@echo
	@echo -e "$(GREEN)$(BOLD)OPTIONS:$(RESET)"
	@echo -e "  $(CYAN)clean$(RESET)     $(DGRAY)$(ARROW)$(RESET) Clean before building"
	@echo -e "  $(CYAN)none$(RESET)      $(DGRAY)$(ARROW)$(RESET) Just build (default)"
	@echo
	@echo -e "$(GREEN)$(BOLD)EXAMPLES:$(RESET)"
ifdef SELECTED_DISTRO
	@echo -e "  $(CYAN)make$(RESET)              $(DGRAY)$(ARROW)$(RESET) Build for detected system ($(YELLOW)$(SELECTED_DISTRO)$(RESET))"
endif
	@echo -e "  $(CYAN)make debian$(RESET)       $(DGRAY)$(ARROW)$(RESET) Build for Debian"
	@echo -e "  $(CYAN)make arch clean$(RESET)   $(DGRAY)$(ARROW)$(RESET) Clean and build for Arch"
	@echo -e "  $(CYAN)make windows$(RESET)      $(DGRAY)$(ARROW)$(RESET) Build for Windows (MSVC)"
	@echo -e "  $(CYAN)make mingw$(RESET)        $(DGRAY)$(ARROW)$(RESET) Build for Windows (MinGW)"
	@echo

# Distribution targets
.PHONY: distros debian arch alpine macos windows mingw
distros: debian arch alpine macos windows mingw

# Define a single build rule to handle all distributions
.PHONY: debian arch alpine macos windows mingw
debian arch alpine macos windows mingw:
	@echo
	@echo -e "$(BG_BLUE)$(WHITE)$(BOLD) BUILD $(RESET) Building for $(BOLD)$(YELLOW)$@$(RESET) distribution..."
	@ACTION=$(word 2, $(MAKECMDGOALS)); \
	if [ "$$ACTION" = "clean" ]; then \
		$(MAKE) -f $(MAKEFILE_LIST) do_clean DISTRO=$@; \
	fi; \
	$(MAKE) -f $(MAKEFILE_LIST) do_build DISTRO=$@

# Default distribution target
.PHONY: default_distro
default_distro:
	@echo
	@echo -e "$(BG_BLUE)$(WHITE)$(BOLD) BUILD $(RESET) Building for default distribution $(BOLD)$(YELLOW)$(DEFAULT_DISTRO)$(RESET)..."
	@ACTION=$(word 2, $(MAKECMDGOALS)); \
	if [ "$$ACTION" = "clean" ]; then \
		$(MAKE) -f $(MAKEFILE_LIST) do_clean DISTRO=$(DEFAULT_DISTRO); \
	fi; \
	$(MAKE) -f $(MAKEFILE_LIST) do_build DISTRO=$(DEFAULT_DISTRO)

# Clean rule for specific distribution
.PHONY: do_clean
do_clean:
	@echo -e "$(BG_BLUE)$(WHITE)$(BOLD) CLEAN $(RESET) Removing build files for $(BOLD)$(YELLOW)$(DISTRO)$(RESET)..."
	@rm -rf $(BUILD_DIR)/$(DISTRO)
	@rm -f $(DISTRO_$(DISTRO)_TARGET)
	@echo -e "$(GREEN)$(CHECK) Cleaned$(RESET) $(DGRAY)$(BUILD_DIR)/$(DISTRO)$(RESET)"
	@echo -e "$(GREEN)$(CHECK) Removed$(RESET) $(DGRAY)$(DISTRO_$(DISTRO)_TARGET)$(RESET)"

# Build rule for specific distribution
.PHONY: do_build
do_build:
	@mkdir -p $(BIN_DIR)
	
	@echo -e "$(BG_GREEN)$(WHITE)$(BOLD) COMPILE $(RESET) Building source files..."
	
	@total_files=`echo "$(SRCS)" | wc -w`; \
	current=0; \
	for src in $(SRCS); do \
		current=$$((current + 1)); \
		percentage=$$((current * 100 / total_files)); \
		obj_dir=$(BUILD_DIR)/$(DISTRO)/`dirname $$src | sed "s|^$(SRC_DIR)/||"`; \
		obj_file=$$obj_dir/`basename $$src .c`.o; \
		mkdir -p $$obj_dir; \
		printf "$(CYAN)[%3d%%]$(RESET) " $$percentage; \
		printf "$(DGRAY)[%3d/%-3d]$(RESET) " $$current $$total_files; \
		echo -e "$(DIM)$$src$(RESET) -> $(DIM)$$obj_file$(RESET)"; \
		$(DISTRO_$(DISTRO)_CC) $(DISTRO_$(DISTRO)_CFLAGS) -c $$src -o $$obj_file || exit 1; \
	done
	
	@echo -e "$(BG_GREEN)$(WHITE)$(BOLD) LINK $(RESET) Creating executable: $(BOLD)$(CYAN)$(DISTRO_$(DISTRO)_TARGET)$(RESET)"
	@mkdir -p $(dir $(DISTRO_$(DISTRO)_TARGET))
	@$(DISTRO_$(DISTRO)_CC) `find $(BUILD_DIR)/$(DISTRO) -name "*.o"` $(DISTRO_$(DISTRO)_LDFLAGS) -o $(DISTRO_$(DISTRO)_TARGET)
	
	@echo
	@echo -e "$(GREEN)$(BOLD)$(CHECK) Build successful!$(RESET)"
	@echo -e "$(GRAY)$(BULLET) Target:$(RESET) $(CYAN)$(DISTRO_$(DISTRO)_TARGET)$(RESET)"
	@echo -e "$(GRAY)$(BULLET) Platform:$(RESET) $(CYAN)$(DISTRO)$(RESET)"
	@echo -e "$(GRAY)$(BULLET) Compiler:$(RESET) $(CYAN)$(DISTRO_$(DISTRO)_CC)$(RESET)"
	@echo
	@echo -e "$(YELLOW)To run:$(RESET) $(BOLD)./$(DISTRO_$(DISTRO)_TARGET)$(RESET)"

# Clean all distributions
.PHONY: clean_all
clean_all:
	@echo -e "$(BG_BLUE)$(WHITE)$(BOLD) CLEAN $(RESET) Removing all build files..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(BIN_DIR)
	@echo -e "$(GREEN)$(CHECK) Cleaned all build directories$(RESET)"

# Run the Lua runtime (for the default distribution)
.PHONY: run
run: default_distro
	@echo
	@echo -e "$(BG_GREEN)$(WHITE)$(BOLD) RUN $(RESET) Executing $(BOLD)$(CYAN)$(DISTRO_$(DEFAULT_DISTRO)_TARGET)$(RESET)..."
	@echo -e "$(DGRAY)$(BULLET) $(ITALIC)Output follows below:$(RESET)"
	@echo
	@$(DISTRO_$(DEFAULT_DISTRO)_TARGET)

# Handle any other arguments by doing nothing
.PHONY: none clean
none clean:
	@: