#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct heap_t;
typedef struct heap_t heap_t;

struct dict_t;
typedef struct dict_t dict_t;

typedef struct node_t {
	unsigned char* value;
	int frequency;
	struct node_t* left_child;
	struct node_t* right_child;
} node_t;

node_t* make_node(unsigned char* value, int frequency, node_t* left_child, node_t* right_child); 
void free_node(node_t* node);
node_t* make_huffman_tree(heap_t* heap);
void print_huffman_tree(node_t* huffman_tree, char parent);
char* add_bit_to_code(const char* base_code, char bit);
void generate_codes(char** codes, node_t* node, const char* code);
node_t* make_huffman_tree_from_dict(dict_t* dictionary);

#endif
