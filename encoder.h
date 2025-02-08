#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "node.h"

void compress(char* dictionary, char* codes[256], FILE *input_file, FILE *output_file);
char* save_dictionary(char* dictionary, FILE *output_file, unsigned char* xor_check);
char* write_first_two_bytes(char* dictionary, FILE *output_file);
void write_n_of_missing_bits(int n_of_missing_bits, FILE *output_file);
void get_eight_bits(char *input_buffer, char *eight_bits);

#endif
