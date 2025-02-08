#ifndef DECODER_H
#define DECODER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "node.h"
#include "dictionary.h"

void give_eight_bit_binary_code(unsigned int byte, char *dest);
unsigned char* get_key(node_t *root, const char *code, int code_length);
int analyze_buffer(unsigned char *buffer_ptr, node_t *root, FILE *output_file);
int check_if_added_unnecessary_keys(node_t *root, char *binary_code);
void decompress(FILE *input_file, FILE *output_file, dict_t *dictionary, node_t *root);
				
#endif