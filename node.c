#include "node.h"
#include "heap.h"

node_t* make_node(unsigned short* value, int frequency, node_t* left_child, node_t* right_child) {
	node_t* node = malloc(sizeof *node);
	
	if (value != NULL) {
		node->value = malloc(sizeof *node->value);
		*(node->value) = *value;
	} else {
		node->value = NULL;
	}
	node->frequency = frequency;
	node->left_child = left_child;
	node->right_child = right_child;
	
	return node;
}

void free_node(node_t* node) {
	if(node != NULL) {
		if (node->left_child != NULL) {
			free_node(node->left_child);
		}
		if (node->right_child != NULL) {
			free_node(node->right_child);
		}
		if(node->value != NULL) {
			free(node->value);
		}
		free(node);
	}
}

node_t* make_huffman_tree(heap_t* heap) {
	if(heap->n_of_elems == 1) {
		return extract_min_value(heap);
	}

	while(heap->n_of_elems > 1) {
		node_t* left_child = extract_min_value(heap);
		node_t* right_child = extract_min_value(heap);
		node_t* parent = make_node(NULL, left_child->frequency + right_child->frequency, left_child, right_child);
		insert(heap, parent);
	}
	return extract_min_value(heap);
}

void print_huffman_tree(node_t* huffman_tree) {
	if (huffman_tree != NULL) {
		printf("%i ", huffman_tree->frequency);
		print_huffman_tree(huffman_tree->left_child);
		print_huffman_tree(huffman_tree->right_child);
	}
}

char* add_bit_to_code(const char* base_code, char bit) {
	int length = strlen(base_code);
	char* new_code = malloc(sizeof *new_code * (length + 2));
	strcpy(new_code, base_code);
	new_code[length] = bit;
	new_code[length + 1] = '\0';
	return new_code;
}

void generate_codes(char** codes, node_t* node, const char* code) {
	if (node->left_child == NULL && node->right_child == NULL) {
		codes[*(node->value)] = strdup(code); 
        	return;
	}
	
	if (node->left_child != NULL) {
		char* left_code = add_bit_to_code(code, '0');
        	generate_codes(codes, node->left_child, left_code);
    		free(left_code);
	}

	if (node->right_child != NULL) {
        	char* right_code = add_bit_to_code(code, '1');
        	generate_codes(codes, node->right_child, right_code);
		free(right_code);    
	}
}
