/**
 * @file dictionary.h
 * @brief Header file for the dictionary module in Huffman coding.
 *
 * This file defines the structures and function prototypes for creating,
 * managing, and parsing the dictionary, which is a string representation
 * of the Huffman tree used for compression and decompression.
 */

#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include <stdlib.h>

#include "node.h"

// Forward declaration
struct node_t;
typedef struct node_t node_t;

/**
 * @struct dict_t
 *
 * @brief Represents the dictionary and metadata extracted from a compressed
 * file.
 */
typedef struct dict_t {
    int n_of_leaves;      ///< Number of leaf nodes in the Huffman tree.
    int n_of_non_leaves;  ///< Number of non-leaf (internal) nodes.
    int n_of_added_bits;  ///< Number of padding bits added to the last byte.
    int n_of_read_bytes;  ///< Number of bytes read for the dictionary header.
    int index_helper;  ///< Index to track position while parsing the dictionary
                       ///< string.
    char* dictionary;  ///< The string representation of the Huffman tree and
                       ///< initial data.
} dict_t;

/**
 * @brief Generates a string representation of the Huffman tree.
 * This string is used as a dictionary for decompression.
 * @param huffman_tree The root of the Huffman tree.
 * @return A dynamically allocated string representing the dictionary.
 */
char* generate_dictionary(const node_t* huffman_tree);

/**
 * @brief Converts an unsigned character to its binary string representation.
 * @param value The character to convert.
 * @param bits The number of bits for the representation (e.g., 8).
 * @return A dynamically allocated string with the binary code.
 */
char* char_binary_code(unsigned char value, size_t bits);

/**
 * @brief Creates and initializes an empty dictionary structure.
 * @return A pointer to the newly created dict_t structure.
 */
dict_t* create_dict();

/**
 * @brief Frees the memory allocated for a dictionary structure.
 * @param dictionary The dictionary to free.
 */
void free_dictionary(dict_t* dictionary);

/**
 * @brief Reads and analyzes the header (prefix) of a compressed file.
 * It extracts metadata like the number of padded bits.
 * @param dictionary The dictionary structure to populate.
 * @param filename The name of the compressed file.
 * @return A string containing the initial part of the dictionary read from the
 * file.
 */
char* analyze_dictionary_prefix(dict_t* dictionary, const char* filename);

/**
 * @brief Traverses the dictionary string to count leaf and non-leaf nodes.
 * @param dictionary The dictionary structure containing the string to analyze.
 * @param index The starting index for the traversal.
 * @return The index in the string after the traversal is complete.
 */
size_t get_dict_stats(dict_t* dictionary, size_t index);

/**
 * @brief Reads the dictionary from the header of a compressed file.
 * @param filename The name of the compressed file.
 * @return A pointer to a populated dict_t structure.
 */
dict_t* get_dictionary_from_file(const char* filename);

/**
 * @brief Checks if the compressed file is valid by computing a XOR checksum.
 * Exits the program if the file is broken.
 * @param filename The name of the file to check.
 */
void check_if_valid_file(const char* filename);

#endif  // _DICTIONARY_H_
