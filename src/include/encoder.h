#ifndef SRC_INCLUDES_ENCODER_H_
#define SRC_INCLUDES_ENCODER_H_

#include "./node.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Compress original file to compressed file
void compress(char *dictionary, const char *codes[256], FILE *input_file,
              FILE *output_file);

// Write dictionary to output file
char *save_dictionary(char *dictionary, FILE *output_file,
                      unsigned char *xor_check);

// Write the first two bytes of the dictionary they aren't xored,
// that's why they are handled differently
char *write_first_two_bytes(char *dictionary, FILE *output_file);

// If there is some leftover codes, pad it with zeros, so it is 8 bits long
void write_n_of_missing_bits(size_t n_of_missing_bits, FILE *output_file);

// Helper function to get the first 8 bits of a string
void get_eight_bits(const char *input_buffer, char *eight_bits);

#endif // SRC_INCLUDES_ENCODER_H_
