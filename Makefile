CC = gcc
# -Werror
CFLAGS = -Wall -Wextra -Wpedantic -O3
OUT = comp.out
SRC = main.c node.c heap.c
OBJ = $(SRC:.c=.o)

comp: clean $(OUT)

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.out *.o

.PHONY: comp clean
