#include "dictionary.h"


char* generate_dictionary(node_t* huffman_tree) {
    char* dictionary = (char*)malloc(12);  // "00000000" + "000" + null terminator
    strcpy(dictionary, "00000000" "000");
    
    node_t* node_stack[256];
    int idx = 0;

    node_stack[idx++] = huffman_tree;

    while (idx > 0) {
        node_t* node = node_stack[--idx];

        if (node->value == NULL) {  
            dictionary = realloc(dictionary, strlen(dictionary) + 2);
            strcat(dictionary, "0");

            node_stack[idx++] = node->right_child;
            node_stack[idx++] = node->left_child;
        } else {
            dictionary = realloc(dictionary, strlen(dictionary) + 2);
            strcat(dictionary, "1");

            char* binary_code = char_binary_code(*(node->value), 8);
            dictionary = realloc(dictionary, strlen(dictionary) + strlen(binary_code) + 1);
            strcat(dictionary, binary_code);
            free(binary_code);
        }
    }
    return dictionary;
}

char* char_binary_code(int value, int bits) {
    char* binary_code = malloc((bits + 1) * sizeof *binary_code);
    binary_code[bits] = '\0';

    for (int i = bits - 1; i >= 0; --i) {
        binary_code[i] = (value & 1) ? '1' : '0';
        value >>= 1;
    }
    return binary_code;
}