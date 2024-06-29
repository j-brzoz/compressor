#ifndef NODE_H
#define NODE_H
#include <stdlib.h>

typedef struct node_t {
	unsigned char value;
	int frequency;
	struct node_t* left_child;
	struct node_t* right_child;
} node_t;

node_t* make_node(unsigned char value, int frequency, node_t* left_child, node_t* right_child); 

#endif
