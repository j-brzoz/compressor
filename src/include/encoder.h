/**
 * @file encoder.h
 * @brief Header file for the Huffman encoding module.
 *
 * This file contains the function prototypes for compressing a file using the
 * Huffman algorithm. It declares the functions needed to process the input
 * file, save the dictionary, and write the compressed data.
 */

#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"

/**
 * @brief Compresses the input file and writes the result to the output file.
 * @param dictionary The string representation of the Huffman tree.
 * @param codes An array of Huffman codes for each character.
 * @param input_file The original file to compress.
 * @param output_file The file to write the compressed data to.
 */
void compress(char* dictionary, const char* codes[256], FILE* input_file,
              FILE* output_file);

/**
 * @brief Writes the dictionary string to the output file.
 * @param dictionary The string representation of the Huffman tree.
 * @param output_file The file to write to.
 * @param xor_check A pointer to the XOR checksum value to be updated.
 * @return A string containing leftover bits from the dictionary that didn't
 * form a full byte.
 */
char* save_dictionary(char* dictionary, FILE* output_file,
                      unsigned char* xor_check);

/**
 * @brief Writes the first two bytes of the dictionary to the file.
 * These bytes (XOR checksum and padding info) are handled separately as they
 * are not xored.
 * @param dictionary The dictionary string.
 * @param output_file The file to write to.
 * @return The remainder of the dictionary string after the first two bytes are
 * processed.
 */
char* write_first_two_bytes(char* dictionary, FILE* output_file);

/**
 * @brief Writes the number of padding bits to the second byte of the output
 * file.
 * @param n_of_missing_bits The number of bits used for padding (0-7).
 * @param output_file The file to write to.
 */
void write_n_of_missing_bits(size_t n_of_missing_bits, FILE* output_file);

/**
 * @brief Helper function to get the first 8 bits from a string.
 * @param input_buffer The source string.
 * @param eight_bits The destination buffer to store the 8 bits.
 */
void get_eight_bits(const char* input_buffer, char* eight_bits);

#endif  // _ENCODER_H_
