CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Werror -O3
# -pg
OUT = comp.out
SRC = main.c node.c heap.c dictionary.c encoder.c decoder.c
OBJ = $(SRC:.c=.o)

comp: clean $(OUT)

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.out *.o 

.PHONY: comp clean
