#include "./includes/decoder.h"

#define BUFFER_LIMIT 4096
#define BYTE_SIZE 8


//  Traverse the Huffman tree using the given code string
const unsigned char *
get_key(
	const node_t *root,
	const char *code,
	size_t code_length
) {
	if (!root || !code) return NULL;

    const node_t *node = root;
    size_t length = 0;

    while (length < code_length && node->left_child) {
        node = (code[length] == '0') ? node->left_child : node->right_child;
        length++;
    }

	// Return the decoded value if found
    return (node && node->value) ? node->value : NULL;
}


// Process the bit–string buffer and write decoded bytes
int
analyze_buffer(
	const unsigned char *buffer,
	const node_t *root,
	FILE *output_file
) {
    if (!buffer || !root || !output_file) return -1;

    size_t buffer_start = 0, buffer_length = strlen((const char *)buffer);
    size_t output_buffer_length = 0;
    unsigned char output_buffer[BUFFER_LIMIT];
    char single_code_buffer[256];

    for (size_t code_length = 1; code_length <= buffer_length - buffer_start;) {
		// Get potential code
        memcpy(single_code_buffer, buffer + buffer_start, code_length);
        single_code_buffer[code_length] = '\0';

		// Find the key
        const unsigned char *key = get_key(
			root,
			single_code_buffer,
			code_length);

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

	// If there is isomething in the output buffer, write it to the file
    if (output_buffer_length > 0) {
        fwrite(output_buffer, 1, output_buffer_length, output_file);
    }

    return (int)(buffer_length - buffer_start);
}


// Verify if unnecessary keys were added
int
check_if_added_unnecessary_keys(
	const node_t *root,
	const char *binary_code
) {
    if (!root || !binary_code) return 0;

    size_t binary_code_length = strlen(binary_code);
    char zeros[BYTE_SIZE + 1] = "0";
    size_t zeros_length = 1;

	// If it is possible to find valid key from zeros padded to compressed file,
	// return the number of added chars.
    while (zeros_length < binary_code_length) {
        const unsigned char *key = get_key(root, zeros, zeros_length);
        if (key) {
            return (int)(binary_code_length / zeros_length);
        }
        zeros[zeros_length++] = '0';
        zeros[zeros_length] = '\0';
    }
    return 0;
}


// Decode input file using Huffman tree and write output
void
decompress(
	FILE *input_file,
	FILE *output_file,
	const dict_t *dictionary,
	const node_t *root
) {
	if (!input_file || !output_file || !dictionary || !root) return;

	fseek(input_file, dictionary->n_of_read_bytes, SEEK_SET);

	size_t binary_string_buffer_length = strlen(
		dictionary->dictionary + dictionary->index_helper);
	unsigned char binary_string_buffer[BUFFER_LIMIT * 32];
	unsigned char bytes_from_input_file[BUFFER_LIMIT];
	char eight_bits[BYTE_SIZE + 1];

	memcpy(
		binary_string_buffer,
		dictionary->dictionary + dictionary->index_helper,
		binary_string_buffer_length);

	size_t read_bytes;
	// Read bytes
	while ((read_bytes = fread(
				bytes_from_input_file,
				1,
				BUFFER_LIMIT,
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

			memcpy(
				binary_string_buffer + binary_string_buffer_length,
				eight_bits,
				BYTE_SIZE);
			binary_string_buffer_length += BYTE_SIZE;
			binary_string_buffer[binary_string_buffer_length] = '\0';
		}

		// Convert binary string to bytes and write to the file
		size_t remaining_bits = analyze_buffer(
			binary_string_buffer,
			root,
			output_file);
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
		char *missing_bits = (char *)calloc(len + 1, sizeof(char));
		if (!missing_bits) {
			fprintf(stderr, "Memory allocation failed\n");
			exit(EXIT_FAILURE);
		}
		memset(missing_bits, '0', len);
		missing_bits[len] = '\0';

		int n_of_unnecessary_keys = check_if_added_unnecessary_keys(
			root,
			missing_bits);

		// Truncate if neccessary
		if (n_of_unnecessary_keys > 0) {
			fflush(output_file);
			if (ftruncate(
					fileno(output_file),
					ftell(output_file) - n_of_unnecessary_keys) != 0) {
				fprintf(stderr, "Error while truncating file\n");
				exit(EXIT_FAILURE);
			}
		}
		free(missing_bits);
	}

	fclose(output_file);
}
