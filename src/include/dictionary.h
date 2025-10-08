#ifndef SRC_INCLUDES_DICTIONARY_H_
#define SRC_INCLUDES_DICTIONARY_H_

#include "./node.h"
#include <stdlib.h>

// Forward declaration
struct node_t;
typedef struct node_t node_t;

typedef struct dict_t {
  int n_of_leaves;
  int n_of_non_leaves;
  int n_of_added_bits;
  int n_of_read_bytes;
  int index_helper;
  char *dictionary;
} dict_t;

// Generates a dictionary from a Huffman tree
char *generate_dictionary(const node_t *huffman_tree);

// Give binary representation of a value in 'bits' bits
char *char_binary_code(unsigned char value, size_t bits);

// Create an empty dictionary
dict_t *create_dict();

// Free a dictionary
void free_dictionary(dict_t *dictionary);

// Get all of the important information from compressed file's header
char *analyze_dictionary_prefix(dict_t *dictionary, const char *filename);

// Traverse dicitionary string and update the number of leaves and non-leaves
size_t get_dict_stats(dict_t *dictionary, size_t index);

// Get dictionary with char encoding from compressed file
dict_t *get_dictionary_from_file(const char *filename);

// Check compressed file validity by computing xor
void check_if_valid_file(const char *filename);

#endif // SRC_INCLUDES_DICTIONARY_H_
