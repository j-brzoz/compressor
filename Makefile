comp:
		@$(CC) main.c node.c heap.c -o comp.out -lm -Wall -Wextra -Wpedantic -O3

clean:
	@rm comp.out
