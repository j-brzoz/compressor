#include "node.h"
#include "heap.h"
#include "dictionary.h"

node_t* make_node(unsigned char* value, int frequency, node_t* left_child, node_t* right_child) {
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

void print_huffman_tree(node_t* huffman_tree, char parent) {
	if (huffman_tree != NULL) {
		if (huffman_tree->value != NULL) {
			printf("%c %i %i\n", parent, huffman_tree->frequency, *(huffman_tree->value));
		} else {
			printf("%c NULL\n", parent);
		}
		print_huffman_tree(huffman_tree->left_child, 'l');
		print_huffman_tree(huffman_tree->right_child, 'r');
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

node_t* make_huffman_tree_from_dict(dict_t* dictionary) {
    char current_bit = dictionary->dictionary[dictionary->index_helper];
    dictionary->index_helper++;
    if (current_bit == '0') {
		node_t* node = make_node(NULL, 0, make_huffman_tree_from_dict(dictionary), make_huffman_tree_from_dict(dictionary));
        return node;
    } else if (current_bit == '1') {
        char* value_str = malloc(8 + 1);  // char length + null terminator
        strncpy(value_str, dictionary->dictionary + dictionary->index_helper, 8);
        value_str[8] = '\0';
        unsigned char parsed = (unsigned char) strtol(value_str, NULL, 2);
        free(value_str);
      
        dictionary->index_helper += 8;
		node_t* node = make_node(&parsed, 0, NULL, NULL);
        return node;
    }
    
    // should never reach here
    return NULL;
}