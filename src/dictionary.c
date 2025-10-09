/**
 * @file dictionary.c
 * @brief Implementation of dictionary generation and parsing for Huffman
 * coding.
 *
 * This file contains functions to create a string representation of the
 * Huffman tree (the "dictionary") to be stored in the compressed file header,
 * and to parse this dictionary from a compressed file to reconstruct the
 * Huffman tree for decompression. It also handles metadata like padding
 * and file validity checks.
 */

#include "dictionary.h"

#define BUFFER_LIMIT 8192

/**
 * @brief Generates a string representation of the Huffman tree.
 * This string is used as a dictionary for decompression.
 * @param huffman_tree The root of the Huffman tree.
 * @return A dynamically allocated string representing the dictionary.
 */
char* generate_dictionary(const node_t* huffman_tree) {
    if (!huffman_tree) return NULL;

    size_t start_dictionary_size = 12;  // "00000000" + "000" + null
    char* dictionary = (char*)malloc(start_dictionary_size);
    if (!dictionary) return NULL;

    // XOR value + number of added bits
    snprintf(dictionary, start_dictionary_size, "%s",
             "00000000"
             "000");

    const node_t* node_stack[256];
    size_t idx = 0;

    node_stack[idx++] = huffman_tree;

    // Traverse and add appropriate bits
    while (idx > 0) {
        const node_t* node = node_stack[--idx];

        char* temp = realloc(dictionary, strlen(dictionary) + 2);
        if (!temp) {
            free(dictionary);
            return NULL;
        }
        dictionary = temp;
        snprintf(dictionary + strlen(dictionary), 2, "%s",
                 node->value ? "1" : "0");

        if (node->value) {
            char* binary_code = char_binary_code(*(node->value), 8);
            if (!binary_code) return NULL;

            temp = realloc(dictionary,
                           strlen(dictionary) + strlen(binary_code) + 1);
            if (!temp) {
                free(binary_code);
                free(dictionary);
                return NULL;
            }
            dictionary = temp;
            snprintf(dictionary + strlen(dictionary), strlen(binary_code) + 1,
                     "%s", binary_code);
            free(binary_code);
        } else {
            node_stack[idx++] = node->left_child;
            node_stack[idx++] = node->right_child;
        }
    }
    return dictionary;
}

/**
 * @brief Converts an unsigned character to its binary string representation.
 * @param value The character to convert.
 * @param bits The number of bits for the representation (e.g., 8).
 * @return A dynamically allocated string with the binary code.
 */
char* char_binary_code(unsigned char value, size_t bits) {
    char* binary_code = malloc(bits + 1);
    if (!binary_code) return NULL;

    binary_code[bits] = '\0';

    for (size_t i = bits; i-- > 0;) {
        binary_code[i] = (value & 1) ? '1' : '0';
        value >>= 1;
    }
    return binary_code;
}

/**
 * @brief Creates and initializes an empty dictionary structure.
 * @return A pointer to the newly created dict_t structure.
 */
dict_t* create_dict() {
    dict_t* dictionary = malloc(sizeof(dict_t));
    if (!dictionary) return NULL;

    dictionary->n_of_leaves = 0;
    dictionary->n_of_non_leaves = 0;
    dictionary->n_of_added_bits = 0;
    dictionary->dictionary = NULL;
    dictionary->n_of_read_bytes = 0;
    dictionary->index_helper = 0;

    return dictionary;
}

/**
 * @brief Frees the memory allocated for a dictionary structure.
 * @param dictionary The dictionary to free.
 */
void free_dictionary(dict_t* dictionary) {
    if (dictionary) {
        free(dictionary->dictionary);
        free(dictionary);
    }
}

/**
 * @brief Reads and analyzes the header (prefix) of a compressed file.
 * It extracts metadata like the number of padded bits.
 * @param dictionary The dictionary structure to populate.
 * @param filename The name of the compressed file.
 * @return A string containing the initial part of the dictionary read from the
 * file.
 */
char* analyze_dictionary_prefix(dict_t* dictionary, const char* filename) {
    if (!dictionary || !filename) return NULL;

    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;

    unsigned char byte_from_input_file[2];
    dictionary->n_of_read_bytes += 2;
    size_t read = fread(byte_from_input_file, 1, 2, file);
    fclose(file);

    check_if_valid_file(filename);

    if (read > 0) {
        // Get number of added bits for padding
        char* binary_code = char_binary_code(byte_from_input_file[1], 8);
        if (!binary_code) return NULL;

        char temp[4] = {0};
        strncpy(temp, binary_code, 3);
        dictionary->n_of_added_bits = (int)strtol(temp, NULL, 2);

        // Return the remainder of the second byte
        char* remaining = strdup(binary_code + 3);
        free(binary_code);
        return remaining;
    }
    return NULL;
}

/**
 * @brief Traverses the dictionary string to count leaf and non-leaf nodes.
 * @param dictionary The dictionary structure containing the string to analyze.
 * @param index The starting index for the traversal.
 * @return The index in the string after the traversal is complete.
 */
size_t get_dict_stats(dict_t* dictionary, size_t index) {
    while (dictionary->n_of_leaves != dictionary->n_of_non_leaves + 1 &&
           index < strlen(dictionary->dictionary)) {
        if (dictionary->dictionary[index] == '0') {
            index++;
            dictionary->n_of_non_leaves++;
        } else if (dictionary->dictionary[index] == '1') {
            index += 9;  // char length = 8 + 1 bit
            dictionary->n_of_leaves++;
        }
    }
    return index;
}

/**
 * @brief Reads the dictionary from the header of a compressed file.
 * @param filename The name of the compressed file.
 * @return A pointer to a populated dict_t structure.
 */
dict_t* get_dictionary_from_file(const char* filename) {
    if (!filename) return NULL;

    size_t prefix_length = 2, dictionary_index, read;
    unsigned char byte_from_input_file[1];

    dict_t* dictionary = create_dict();
    if (!dictionary) return NULL;

    dictionary->dictionary = analyze_dictionary_prefix(dictionary, filename);
    dictionary_index = get_dict_stats(dictionary, 0);

    // skip the prefix bytes
    FILE* file = fopen(filename, "rb");
    if (!file) {
        free_dictionary(dictionary);
        return NULL;
    }
    fseek(file, prefix_length, SEEK_SET);

    while (dictionary->n_of_leaves != dictionary->n_of_non_leaves + 1 &&
           (read = fread(byte_from_input_file, 1, 1, file)) > 0) {
        dictionary->n_of_read_bytes++;

        char* bin_code = char_binary_code(byte_from_input_file[0], 8);
        if (!bin_code) {
            fclose(file);
            free_dictionary(dictionary);
            return NULL;
        }

        size_t current_length = strlen(dictionary->dictionary);
        size_t bin_length = strlen(bin_code);
        size_t new_size = current_length + bin_length + 1;

        dictionary->dictionary = realloc(dictionary->dictionary, new_size);
        if (!dictionary->dictionary) {
            free(bin_code);
            fclose(file);
            free_dictionary(dictionary);
            return NULL;
        }

        snprintf(dictionary->dictionary + current_length, bin_length + 1, "%s",
                 bin_code);
        free(bin_code);
        dictionary_index = get_dict_stats(dictionary, dictionary_index);
    }

    if (dictionary_index > (size_t)strlen(dictionary->dictionary)) {
        read = fread(byte_from_input_file, 1, 1, file);
        if (read > 0) {
            dictionary->n_of_read_bytes++;

            char* bin_code = char_binary_code(byte_from_input_file[0], 8);
            if (!bin_code) {
                fclose(file);
                free_dictionary(dictionary);
                return NULL;
            }

            size_t current_length = strlen(dictionary->dictionary);
            size_t bin_length = strlen(bin_code);
            size_t new_size = current_length + bin_length + 1;
            dictionary->dictionary = realloc(dictionary->dictionary, new_size);
            if (!dictionary->dictionary) {
                free(bin_code);
                fclose(file);
                free_dictionary(dictionary);
                return NULL;
            }

            snprintf(dictionary->dictionary + current_length, bin_length + 1,
                     "%s", bin_code);
            free(bin_code);
        }
    }
    fclose(file);
    return dictionary;
}

/**
 * @brief Checks if the compressed file is valid by computing a XOR checksum.
 * Exits the program if the file is broken.
 * @param filename The name of the file to check.
 */
void check_if_valid_file(const char* filename) {
    if (!filename) {
        fprintf(stderr, "Invalid filename\n");
        exit(1);
    }

    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(1);
    }

    // Get xor value
    unsigned char first_two_bytes[2];
    size_t read_first = fread(first_two_bytes, 1, 2, file);
    if (read_first < 2) {
        fprintf(stderr, "File too short: %s\n", filename);
        fclose(file);
        exit(1);
    }

    unsigned char xor = first_two_bytes[0];

    // Compute xor
    unsigned char bytes_from_input_file[BUFFER_LIMIT];
    size_t read;
    while ((read = fread(bytes_from_input_file, 1, BUFFER_LIMIT, file)) > 0) {
        for (size_t i = 0; i < read; i++) {
            xor ^= bytes_from_input_file[i];
        }
    }
    fclose(file);

    // 'a' is the original value
    if (xor != 'a') {
        fprintf(stderr, "Broken file: %s\n", filename);
        exit(1);
    }
}
