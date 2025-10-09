/**
 * @file decoder.c
 * @brief Implementation of the Huffman decoding and decompression logic.
 *
 * This file contains the functions necessary to decompress a file that was
 * compressed using the corresponding Huffman encoder. It reads the Huffman tree
 * representation (dictionary) from the compressed file, reconstructs the tree,
 * and then uses it to decode the compressed data stream back into the original
 * file.
 */

#include "decoder.h"

#define BUFFER_LIMIT 4096
#define BYTE_SIZE 8

/**
 * @brief Traverse the Huffman tree using the given code string to find a
 * character.
 * @param root The root of the Huffman tree.
 * @param code The binary code string to search for.
 * @param code_length The length of the code to consider.
 * @return A pointer to the decoded character if found, otherwise NULL.
 */
const unsigned char* get_key(const node_t* root, const char* code,
                             size_t code_length) {
    if (!root || !code) return NULL;

    const node_t* node = root;
    size_t length = 0;

    while (length < code_length && node->left_child) {
        node = (code[length] == '0') ? node->left_child : node->right_child;
        length++;
    }

    // Return the decoded value if found
    return (node && node->value) ? node->value : NULL;
}

/**
 * @brief Process a buffer of binary string, decode it, and write the result to
 * a file.
 * @param buffer The input buffer containing the binary string.
 * @param root The root of the Huffman tree.
 * @param output_file The file to write the decoded bytes to.
 * @return The number of remaining bits in the buffer that were not processed.
 */
int analyze_buffer(const unsigned char* buffer, const node_t* root,
                   FILE* output_file) {
    if (!buffer || !root || !output_file) return -1;

    size_t buffer_start = 0, buffer_length = strlen((const char*)buffer);
    size_t output_buffer_length = 0;
    unsigned char output_buffer[BUFFER_LIMIT];
    char single_code_buffer[256];

    for (size_t code_length = 1; code_length <= buffer_length - buffer_start;) {
        // Get potential code
        memcpy(single_code_buffer, buffer + buffer_start, code_length);
        single_code_buffer[code_length] = '\0';

        // Find the key
        const unsigned char* key =
            get_key(root, single_code_buffer, code_length);

        // If found the key, add to the output buffer
        if (key) {
            output_buffer[output_buffer_length++] = *key;
            buffer_start += code_length;
            code_length = 1;

            if (output_buffer_length == BUFFER_LIMIT) {
                fwrite(output_buffer, 1, output_buffer_length, output_file);
                output_buffer_length = 0;
            }
        } else {
            code_length++;
        }
    }

    // If there is something in the output buffer, write it to the file
    if (output_buffer_length > 0) {
        fwrite(output_buffer, 1, output_buffer_length, output_file);
    }

    return (int)(buffer_length - buffer_start);
}

/**
 * @brief Checks if padding zeros at the end of the file could be misinterpreted
 * as valid codes.
 * @param root The root of the Huffman tree.
 * @param binary_code The binary string representing the padded bits.
 * @return The number of characters that would be incorrectly decoded from the
 * padding.
 */
int check_if_added_unnecessary_keys(const node_t* root,
                                    const char* binary_code) {
    if (!root || !binary_code) return 0;

    size_t binary_code_length = strlen(binary_code);
    char zeros[BYTE_SIZE + 1] = "0";
    size_t zeros_length = 1;

    // If it is possible to find valid key from zeros padded to compressed file,
    // return the number of added chars.
    while (zeros_length < binary_code_length) {
        const unsigned char* key = get_key(root, zeros, zeros_length);
        if (key) {
            return (int)(binary_code_length / zeros_length);
        }
        zeros[zeros_length++] = '0';
        zeros[zeros_length] = '\0';
    }
    return 0;
}

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
                const node_t* root) {
    if (!input_file || !output_file || !dictionary || !root) return;

    fseek(input_file, dictionary->n_of_read_bytes, SEEK_SET);

    size_t binary_string_buffer_length =
        strlen(dictionary->dictionary + dictionary->index_helper);
    unsigned char binary_string_buffer[BUFFER_LIMIT * 32];
    unsigned char bytes_from_input_file[BUFFER_LIMIT];
    char eight_bits[BYTE_SIZE + 1];

    memcpy(binary_string_buffer,
           dictionary->dictionary + dictionary->index_helper,
           binary_string_buffer_length);

    size_t read_bytes;
    // Read bytes
    while ((read_bytes = fread(bytes_from_input_file, 1, BUFFER_LIMIT,
                               input_file)) > 0) {
        for (size_t i = 0; i < read_bytes; i++) {
            // Convert to binary
            unsigned int value = bytes_from_input_file[i];
            for (int j = BYTE_SIZE - 1; j >= 0; --j) {
                eight_bits[j] = (value & 1) ? '1' : '0';
                value >>= 1;
            }
            eight_bits[BYTE_SIZE] = '\0';

            if (binary_string_buffer_length + BYTE_SIZE >=
                sizeof(binary_string_buffer)) {
                fprintf(stderr, "Binary string buffer overflow\n");
                exit(EXIT_FAILURE);
            }

            memcpy(binary_string_buffer + binary_string_buffer_length,
                   eight_bits, BYTE_SIZE);
            binary_string_buffer_length += BYTE_SIZE;
            binary_string_buffer[binary_string_buffer_length] = '\0';
        }

        // Convert binary string to bytes and write to the file
        size_t remaining_bits =
            analyze_buffer(binary_string_buffer, root, output_file);
        memmove(
            binary_string_buffer,
            binary_string_buffer + binary_string_buffer_length - remaining_bits,
            remaining_bits);
        binary_string_buffer_length = remaining_bits;
    }

    fclose(input_file);
    fflush(output_file);

    // Handle potential padded bits
    if (dictionary->n_of_added_bits > 0) {
        size_t len = dictionary->n_of_added_bits;
        char* missing_bits = (char*)calloc(len + 1, sizeof(char));
        if (!missing_bits) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        memset(missing_bits, '0', len);
        missing_bits[len] = '\0';

        int n_of_unnecessary_keys =
            check_if_added_unnecessary_keys(root, missing_bits);

        // Truncate if neccessary
        if (n_of_unnecessary_keys > 0) {
            fflush(output_file);
            if (ftruncate(fileno(output_file),
                          ftell(output_file) - n_of_unnecessary_keys) != 0) {
                fprintf(stderr, "Error while truncating file\n");
                exit(EXIT_FAILURE);
            }
        }
        free(missing_bits);
    }

    fclose(output_file);
}
