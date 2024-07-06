#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>

struct heap_t;
typedef struct heap_t heap_t;

typedef struct node_t {
	unsigned short* value;
	int frequency;
	struct node_t* left_child;
	struct node_t* right_child;
} node_t;

node_t* make_node(unsigned short* value, int frequency, node_t* left_child, node_t* right_child); 
void free_node(node_t* node);
node_t* make_huffman_tree(heap_t* heap);
void print_huffman_tree(node_t* huffman_tree);

#endif
