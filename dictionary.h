#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdlib.h>
#include "node.h"

char* generate_dictionary(node_t* huffman_tree);
char* char_binary_code(int value, int bits);

#endif
