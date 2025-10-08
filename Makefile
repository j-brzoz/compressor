CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Werror -O3 -I src/include
LDFLAGS =

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c,bin/%.o,$(SRC))
BIN_DIR = bin
BIN = $(BIN_DIR)/comp.out

all: $(BIN)

$(BIN): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

bin/%.o: src/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

clean:
	@rm -f $(BIN_DIR)/*.o
	@rm -f $(BIN)

lint:
	cppcheck --force --enable=all --inconclusive --std=c99 -Isrc/include \
		--suppress=missingIncludeSystem src

format:
	clang-format -i $(shell find . -name '*.c' -o -name '*.h')

test: $(BIN)
	@echo "Running compression tests..."
	@$(BIN) 0 test/test.txt small-txt.huff
	@$(BIN) 1 small-txt.huff small-txt.txt
	@cmp -s test/test.txt small-txt.txt && echo "SAME - short txt" || echo "DIFF - short txt"
	@echo
	@$(BIN) 0 test/pan-tadeusz.txt big-txt.huff
	@$(BIN) 1 big-txt.huff big-txt.txt
	@cmp -s test/pan-tadeusz.txt big-txt.txt && echo "SAME - long txt" || echo "DIFF - long txt"
	@echo
	@$(BIN) 0 test/test.png png.huff
	@$(BIN) 1 png.huff png.png
	@cmp -s test/test.png png.png && echo "SAME - png" || echo "DIFF - png"
	@echo
	@rm -f *.huff png.png big-txt.txt small-txt.txt
	@echo "Tests finished."

.PHONY: all clean lint format test