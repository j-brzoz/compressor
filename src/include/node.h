#ifndef SRC_INCLUDES_NODE_H_
#define SRC_INCLUDES_NODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct heap_t;  // Forward declaration
typedef struct heap_t heap_t;

struct dict_t;  // Forward declaration
typedef struct dict_t dict_t;

typedef struct node_t {
	unsigned char *value;
	int frequency;
	struct node_t *left_child;
	struct node_t *right_child;
} node_t;


// Make node with given parameters
node_t *
make_node(
	const unsigned char *value,
	int frequency,
	node_t *left_child,
	node_t *right_child
);


// Free node structure
void
free_node(
	node_t *node
);


// Make Huffman tree from heap
node_t *
make_huffman_tree(
	heap_t *heap
);


// Helper for debugging
void
print_huffman_tree(
	const node_t *huffman_tree,
	char parent
);


// Append bit to binary string
char *
add_bit_to_code(
	const char *base_code,
	char bit
);


// Fill codes array from traversing Huffman tree
void
generate_codes(
	const char **codes,
	const node_t *node,
	const char *code
);


// Make huffman tree from dictionary
node_t *
make_huffman_tree_from_dict(
	dict_t *dictionary
);

#endif  // SRC_INCLUDES_NODE_H_
