#include "node.h"

node_t* make_node(unsigned char value, int frequency, node_t* left_child, node_t* right_child) {
	node_t* heap_node = malloc(sizeof *heap_node);
	heap_node->value = value;
	heap_node->frequency = frequency;
	heap_node->left_child = left_child;
	heap_node->right_child = right_child;
	
	return heap_node;
}
