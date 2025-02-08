#include "decoder.h"
#include "dictionary.h"

#define BUFFER_LIMIT 4096


/* --- get_key: Traverse the Huffman tree using the given code string --- */
unsigned char* get_key(node_t *root, const char *code, int code_length) {
    int length = 0;
    node_t *node = root;
    if (node->left_child != NULL) {
        while (length < code_length) {
            if (node->left_child != NULL) {
                if (code[length] == '0') {
                    node = node->left_child;
                } else {
                    node = node->right_child;
                }
                length++;
            } else {
                return node->value;
            }
        }
        return (node->value != NULL) ? node->value : NULL;
    } else {
        return (code[0] == '1') ? node->value : NULL;
    }
}

/* --- analyze_buffer: Process the bit–string buffer and write decoded bytes --- */
int analyze_buffer(unsigned char *buffer, node_t *root, FILE *output_file) {
    int code_length = 1;
    unsigned char* key;

    unsigned char output_buffer[BUFFER_LIMIT];
    int output_buffer_length = 0;

	char single_code_buffer[256];
    
	int buffer_start = 0;
	int buffer_length = (int)strlen((char *)buffer);

    while (code_length <= buffer_length-buffer_start) {
        strncpy(single_code_buffer, (char*)(buffer+buffer_start), code_length);
        key = get_key(root, single_code_buffer, code_length);

        if (key != NULL) {
            // a valid key was found
            buffer_start += code_length;;
            output_buffer[output_buffer_length++] = *key;
            code_length = 1;

            if (output_buffer_length == BUFFER_LIMIT) {
                fwrite(output_buffer, 1, output_buffer_length, output_file);
                output_buffer_length = 0;
            }
        } else {
            code_length++;
        }
    }
    if (output_buffer_length > 0) {
        fwrite(output_buffer, 1, output_buffer_length, output_file);
    }
	return buffer_length-buffer_start;
}


int check_if_added_unnecessary_keys(node_t *root, char *binary_code) {
    int binary_code_length = (int)strlen(binary_code);
    
    char zeros[binary_code_length + 1];
    zeros[0] = '0';
    zeros[1] = '\0';
    
    int zeros_length = 1;
    int n_of_unnecessary_keys = 0;

    while (zeros_length < binary_code_length) {
        unsigned char *key = get_key(root, zeros, zeros_length);
        if (key != NULL) {
            n_of_unnecessary_keys += binary_code_length / zeros_length;
            return n_of_unnecessary_keys;
        } else {
            zeros[zeros_length] = '0';
            zeros[zeros_length + 1] = '\0';
            zeros_length += 1;
        }
    }
    return n_of_unnecessary_keys;
}

/* --- decompress:
         Reads bytes from the input file, converts them to an
         eight–bit binary string appended to a buffer, decodes codes
         from the buffer via the Huffman tree, and writes the decoded
         bytes to the output file. After processing, if missing padded
         bits were added, truncates the output file accordingly. --- */
void decompress(FILE *input_file, FILE *output_file, dict_t *dictionary, node_t *root) {

    fseek(input_file, dictionary->n_of_read_bytes, SEEK_SET);

	char* initial_buffer = dictionary->dictionary+dictionary->index_helper;

    /* Initialize the bit buffer with the initial data from the Huffman tree */
	unsigned char binary_string_buffer[BUFFER_LIMIT*32];

	memcpy(binary_string_buffer, initial_buffer, strlen(initial_buffer));
	int binary_string_buffer_length = strlen(initial_buffer);


    unsigned char bytes_from_input_file[BUFFER_LIMIT];
    int read_bytes;

	int byte_size = 8;
    while ((read_bytes = fread(bytes_from_input_file, 1, BUFFER_LIMIT, input_file)) > 0) {
        /* For each byte, append its eight–bit binary representation */
        for (int i = 0; i < read_bytes; i++) {
            char eight_bits[byte_size+1];
			eight_bits[byte_size] = '\0';
			unsigned int value = (unsigned int)bytes_from_input_file[i];
			for (int j = byte_size - 1; j >= 0; --j) {
				eight_bits[j] = (value & 1) ? '1' : '0';
				value >>= 1;
			}

			if (binary_string_buffer_length + byte_size >= BUFFER_LIMIT * 32) {
                fprintf(stderr, "Binary string buffer overflow: %d + %d >= %d\n", binary_string_buffer_length, byte_size, BUFFER_LIMIT*16);
                exit(1);
            }
			memcpy(binary_string_buffer + binary_string_buffer_length, eight_bits, byte_size);
            binary_string_buffer_length += byte_size;
			binary_string_buffer[binary_string_buffer_length] = '\0';
        }
        int remainging_bits = analyze_buffer(binary_string_buffer, root, output_file);
		memcpy(binary_string_buffer, binary_string_buffer+binary_string_buffer_length-remainging_bits, remainging_bits);
		binary_string_buffer_length = remainging_bits;
    }
    // int remainging_bits = analyze_buffer(binary_string_buffer, root, output_file);
	// memcpy(binary_string_buffer, binary_string_buffer+binary_string_buffer_length-remainging_bits, remainging_bits);
	// binary_string_buffer_length = remainging_bits;
    fclose(input_file);
    fflush(output_file);

    /* If there were missing padded bits, check for and remove unnecessary keys */
    if (dictionary->n_of_added_bits != 0) {
        int len = dictionary->n_of_added_bits;
        char *missing_bits = (char *)malloc(len + 1);

        memset(missing_bits, '0', len); // because the padding was all zeros
        missing_bits[len] = '\0';

        int n_of_unnecessary_keys = check_if_added_unnecessary_keys(root, missing_bits);
        if (n_of_unnecessary_keys > 0) {
            fflush(output_file);
            int file_descriptor = fileno(output_file);
            long position = ftell(output_file);
			if (ftruncate(file_descriptor, position - n_of_unnecessary_keys) != 0) {
                fprintf(stderr, "Error while truncating file");
                exit(1);
            }
		}
		free(missing_bits);
    }
    fclose(output_file);
}