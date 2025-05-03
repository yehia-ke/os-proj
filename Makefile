# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -I. `pkg-config --cflags gtk+-3.0` -rdynamic
LDFLAGS = `pkg-config --libs gtk+-3.0`

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source files and object files
SRC_FILES := $(filter-out $(SRC_DIR)/main\ copy.c $(SRC_DIR)/main\ copy\ 2.c, $(wildcard $(SRC_DIR)/**/*.c $(SRC_DIR)/*.c))
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Output binary
TARGET = $(BIN_DIR)/program

# Default rule
all: $(TARGET)

# Link object files into the final binary
$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug the program with GDB
debug: $(TARGET)
	gdb $(TARGET)

.PHONY: all clean run debug