/**
 * @file decoder.h
 * @brief Header file for the Huffman decoding module.
 *
 * This file contains the function prototypes for decompressing a file that was
 * encoded using the Huffman algorithm. It declares the functions needed to
 * parse the compressed data and reconstruct the original file.
 */

#ifndef _DECODER_H_
#define _DECODER_H_

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dictionary.h"
#include "node.h"

/**
 * @brief Traverse the Huffman tree using the given code string to find a
 * character.
 * @param root The root of the Huffman tree.
 * @param code The binary code string to search for.
 * @param code_length The length of the code to consider.
 * @return A pointer to the decoded character if found, otherwise NULL.
 */
const unsigned char* get_key(const node_t* root, const char* code,
                             size_t code_length);

/**
 * @brief Process a buffer of binary string, decode it, and write the result to
 * a file.
 * @param buffer The input buffer containing the binary string.
 * @param root The root of the Huffman tree.
 * @param output_file The file to write the decoded bytes to.
 * @return The number of remaining bits in the buffer that were not processed.
 */
int analyze_buffer(const unsigned char* buffer, const node_t* root,
                   FILE* output_file);

/**
 * @brief Checks if padding zeros at the end of the file could be misinterpreted
 * as valid codes.
 * @param root The root of the Huffman tree.
 * @param binary_code The binary string representing the padded bits.
 * @return The number of characters that would be incorrectly decoded from the
 * padding.
 */
int check_if_added_unnecessary_keys(const node_t* root,
                                    const char* binary_code);

/**
 * @brief Decompresses an input file using the provided Huffman tree and
 * dictionary.
 * @param input_file The compressed file to read from.
 * @param output_file The file to write the decompressed data to.
 * @param dictionary The dictionary structure containing metadata from the
 * compressed file.
 * @param root The root of the Huffman tree used for decoding.
 */
void decompress(FILE* input_file, FILE* output_file, const dict_t* dictionary,
                const node_t* root);

#endif  // _DECODER_H_
