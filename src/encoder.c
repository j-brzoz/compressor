#include "./includes/encoder.h"
#include "./includes/dictionary.h"

#define BUFFER_LIMIT 8192
#define BYTE_SIZE 8


// Compress original file to compressed file
void
compress(
	char *dictionary,
	const char *codes[256],
	FILE *input_file,
	FILE *output_file
) {
    unsigned char *xor_check = malloc(sizeof(unsigned char));
    if (!xor_check) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    *xor_check = (unsigned char)'a';  // 'magic number' used later to check
									  // validity

    unsigned char input_buffer[BUFFER_LIMIT * 9],
		output_buffer[BUFFER_LIMIT],
		bytes_from_input_file[BUFFER_LIMIT];
    size_t input_buffer_length = 0, output_buffer_length = 0, read;

    char *leftover_data = save_dictionary(dictionary, output_file, xor_check);

    if (leftover_data) {
        size_t leftover_length = strlen(leftover_data);
        memcpy(input_buffer, leftover_data, leftover_length);
        input_buffer_length = leftover_length;
        free((void *)leftover_data);
    }

	// Read original file
    while ((read = fread(
				bytes_from_input_file,
				1,
				BUFFER_LIMIT,
				input_file)) > 0) {
        for (size_t i = 0; i < read; i++) {
			// Convert bytes to codes and save to buffer
            size_t code_index = (unsigned char)bytes_from_input_file[i];
            const char *code = codes[code_index];
            size_t code_length = strlen(code);

            if (input_buffer_length + code_length >= BUFFER_LIMIT * 9) {
                fprintf(
					stderr,
					"Input buffer overflow: %zu + %zu >= %d\n",
					input_buffer_length,
					code_length,
					BUFFER_LIMIT * 9);
                exit(EXIT_FAILURE);
            }

            memcpy(input_buffer + input_buffer_length, code, code_length);
            input_buffer_length += code_length;
        }

		// Write to file if full buffer
		size_t iteration_counter = 0;
        while (input_buffer_length >= BYTE_SIZE) {
            unsigned char byte = 0;
            for (size_t i = 0; i < BYTE_SIZE; i++) {
                byte = (byte << 1) |
					(input_buffer[iteration_counter * BYTE_SIZE + i] - '0');
            }
            input_buffer_length -= BYTE_SIZE;

            output_buffer[output_buffer_length++] = byte;
            *xor_check ^= byte;

            if (output_buffer_length == BUFFER_LIMIT) {
                fwrite(output_buffer, 1, output_buffer_length, output_file);
                output_buffer_length = 0;
            }
            iteration_counter++;
        }
        memmove(
			input_buffer,
			input_buffer + iteration_counter*BYTE_SIZE,
			input_buffer_length);
    }

	// Add missing bits
    if (input_buffer_length > 0) {
        unsigned char byte = 0;
        for (size_t i = 0; i < input_buffer_length; i++) {
            byte = (byte << 1) | (input_buffer[i] - '0');
        }
        byte <<= (BYTE_SIZE - input_buffer_length);  // Pad with zeros
        *xor_check ^= byte;

        output_buffer[output_buffer_length++] = byte;
    }

    if (output_buffer_length > 0) {
        fwrite(output_buffer, 1, output_buffer_length, output_file);
    }

    fclose(input_file);

	// Write xor
	rewind(output_file);
	fwrite(xor_check, 1, 1, output_file);
	free(xor_check);

	write_n_of_missing_bits(BYTE_SIZE - input_buffer_length, output_file);
	fclose(output_file);
}


// Write dictionary to output file
char *
save_dictionary(
	char *dictionary,
	FILE *output_file,
	unsigned char *xor_check
) {
    char *eight_bits = malloc(9);
    if (!eight_bits) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    unsigned char output_buffer[BUFFER_LIMIT];
    size_t output_buffer_length = 0;

	// Treating them differently, because they are not xored
    char *rest_of_the_dictionary = write_first_two_bytes(
		dictionary,
		output_file);
    while (strlen(rest_of_the_dictionary) >= BYTE_SIZE) {
        get_eight_bits(rest_of_the_dictionary, eight_bits);
        output_buffer[output_buffer_length] = (unsigned char)strtol(
			eight_bits,
			NULL,
			2);

        *xor_check ^= output_buffer[output_buffer_length];
        output_buffer_length++;

        memmove(
			rest_of_the_dictionary,
			rest_of_the_dictionary + BYTE_SIZE,
			strlen(rest_of_the_dictionary) + 1 - BYTE_SIZE);

        if (output_buffer_length == BUFFER_LIMIT) {
            fwrite(output_buffer, 1, output_buffer_length, output_file);
            output_buffer_length = 0;
        }
    }
    fwrite(output_buffer, 1, output_buffer_length, output_file);
    fflush(output_file);
	free(eight_bits);
	return rest_of_the_dictionary;
}


// Write the first two bytes of the dictionary
// they aren't xored, that's why they are handled differently
char *
write_first_two_bytes(
	char *dictionary,
	FILE *output_file
) {
	char *eight_bits = malloc(BYTE_SIZE + 1);
	if (!eight_bits) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

	unsigned char output_buffer[2];
    int output_buffer_length = 0;

    if (strlen(dictionary) >= BYTE_SIZE * 2) {
        for (size_t  i = 0; i < 2; i++) {
            get_eight_bits(dictionary, eight_bits);
            output_buffer[output_buffer_length++] = (unsigned char)strtol(
				eight_bits,
				NULL,
				2);
			memmove(
				dictionary,
				dictionary + BYTE_SIZE,
				strlen(dictionary) - BYTE_SIZE + 1);
        }
        fwrite(output_buffer, 1, output_buffer_length, output_file);
    }
    fflush(output_file);
	free(eight_bits);
    return dictionary;
}


// If there is some leftover codes, pad it with zeros, so it is 8 bits long
void
write_n_of_missing_bits(
	size_t n_of_missing_bits,
	FILE *output_file
) {
    unsigned char second_byte[1];
    if (fread(second_byte, 1, 1, output_file) != 1) {
        fprintf(stderr, "Error reading file.\n");
        exit(EXIT_FAILURE);
    }

    char *binary_string = char_binary_code(second_byte[0], BYTE_SIZE);
    char *binary_string_of_n_of_missing_bits = char_binary_code(
		(unsigned char)n_of_missing_bits,
		3);

    // Replace the first 3 bits of the original code with the last 3 bits
	// of the n_of_missing_bits code
    strncpy(binary_string, binary_string_of_n_of_missing_bits, 3);

	int value = (int)strtol(binary_string, NULL, 2);
	unsigned char byte = (unsigned char)value;

	fseek(output_file, 1, SEEK_SET);
	fwrite(&byte, 1, 1, output_file);
	free(binary_string);
	free(binary_string_of_n_of_missing_bits);
}


// Helper function to get the first 8 bits of a string
void
get_eight_bits(
	const char *input_buffer,
	char *eight_bits
) {
    strncpy(eight_bits, input_buffer, BYTE_SIZE);
    eight_bits[BYTE_SIZE] = '\0';
}
