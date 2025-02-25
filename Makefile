# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -I$(SRC_DIR)/headers -I/mingw64/include/lua5.4
LDFLAGS = -L/mingw64/lib -llua

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = binout

# Target executable
TARGET = $(BIN_DIR)/reflex

# Find all C source files recursively in src/
SRCS := $(shell find $(SRC_DIR) -type f -name "*.c")

# Extract unique directories containing source files
DIRS := $(sort $(dir $(SRCS)))
BUILD_DIRS := $(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(DIRS))

# Convert source files into object files in build/
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Default target
all: $(BUILD_DIRS) $(BIN_DIR) $(TARGET)

# Ensure all subdirectories in build exist before compiling
$(BUILD_DIRS):
	mkdir -p $@

# Ensure bin directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile .c files to .o (ensuring subfolders exist first)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile the final program
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

# Clean generated files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run the Lua runtime
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
