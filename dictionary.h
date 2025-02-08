#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdlib.h>
#include "node.h"

typedef struct dict_t{
    int n_of_leaves;
    int n_of_non_leaves;
    int n_of_added_bits;
    int n_of_read_bytes;
	int index_helper;
    char *dictionary;
} dict_t;

char* generate_dictionary(node_t* huffman_tree);
char* char_binary_code(unsigned char value, int bits);
dict_t* create_dict();
void free_dictionary(dict_t* dictionary);
char* analyze_dictionary_prefix(dict_t* dictionary, char* filename);
int get_dict_stats(dict_t* dictionary, int index);
dict_t* get_dictionary_from_file(char* filename);
void check_if_valid_file(char* filename);

#endif
