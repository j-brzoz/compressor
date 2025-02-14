CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Werror -O3 -I src/includes
LDFLAGS =
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
OUT = $(BIN_DIR)/comp.out
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

# Default target
all: $(OUT)

# Link the final executable
$(OUT): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm $(BUILD_DIR)/*.o $(BIN_DIR)/*.out

.PHONY: all clean
