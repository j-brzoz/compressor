#ifndef SRC_INCLUDES_DECODER_H_
#define SRC_INCLUDES_DECODER_H_

#include "./dictionary.h"
#include "./node.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//  Traverse the Huffman tree using the given code string
const unsigned char *get_key(const node_t *root, const char *code,
                             size_t code_length);

// Process the bit–string buffer and write decoded bytes
int analyze_buffer(const unsigned char *buffer, const node_t *root,
                   FILE *output_file);

// Verify if unnecessary keys were added
int check_if_added_unnecessary_keys(const node_t *root,
                                    const char *binary_code);

// Decode input file using Huffman tree and write output
void decompress(FILE *input_file, FILE *output_file, const dict_t *dictionary,
                const node_t *root);

#endif // SRC_INCLUDES_DECODER_H_
