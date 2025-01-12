#include "encoder.h"

#define BUFFER_LIMIT 8096


// Compress function implementation
void compress(char* dictionary, char* codes[256], FILE *input_file, FILE *output_file) {
	unsigned char* xor_check = malloc(sizeof(unsigned char));
	*xor_check = (unsigned char)'a';

    unsigned char input_buffer[BUFFER_LIMIT*9];
    int input_buffer_length = 0;  

    unsigned char output_buffer[BUFFER_LIMIT];
    int output_buffer_length = 0;

    unsigned char bytes_from_input_file[BUFFER_LIMIT];
    int read;

    char* leftover_data = save_dictionary(dictionary, output_file, xor_check);
    if (leftover_data) {
        int leftover_length = strlen(leftover_data);
        memcpy(input_buffer, leftover_data, leftover_length);
        input_buffer_length = leftover_length;
        free(leftover_data);
    }

    while ((read = fread(bytes_from_input_file, 1, BUFFER_LIMIT, input_file)) > 0) {
        for (int i = 0; i < read; i++) {
            int code_index = (unsigned char)bytes_from_input_file[i];
            const char *code = codes[code_index];
            int code_length = strlen(code);

            if (input_buffer_length + code_length >= BUFFER_LIMIT * 9) {
                fprintf(stderr, "Input buffer overflow: %d + %d >= %d\n", input_buffer_length, code_length, BUFFER_LIMIT*8);
                exit(1);
            }

            memcpy(input_buffer + input_buffer_length, code, code_length);
            input_buffer_length += code_length;		
        }

		int iteration_counter = 0;
        while (input_buffer_length > 8) {
            unsigned char byte = 0;
            for (int i = 0; i < 8; i++) {
                byte = (byte << 1) | (input_buffer[iteration_counter*8+i] - '0');
            }
			input_buffer_length -= 8;

            output_buffer[output_buffer_length++] = byte;
            *xor_check  ^= byte;

            if (output_buffer_length == BUFFER_LIMIT) {
                fwrite(output_buffer, 1, output_buffer_length, output_file);
                output_buffer_length = 0;
            }
			iteration_counter++;
        }
        memmove(input_buffer, input_buffer + iteration_counter*8, input_buffer_length);

        fwrite(output_buffer, 1, output_buffer_length, output_file);
        output_buffer_length = 0;
    }

	// Add missing bits
    if (input_buffer_length > 0) {
        unsigned char byte = 0;
        for (int i = 0; i < input_buffer_length; i++) {
            byte = (byte << 1) | (input_buffer[i] - '0');
        }
        byte <<= (8 - input_buffer_length); // Pad with zeros
        *xor_check ^= byte;

        output_buffer[output_buffer_length++] = byte;
    }

    if (output_buffer_length > 0) {
        fwrite(output_buffer, 1, output_buffer_length, output_file);
		output_buffer_length = 0;
    }
    write_xor_check(output_file, xor_check);
    fclose(input_file);
    write_n_of_missing_bits(8 - input_buffer_length, output_file);
	fclose(output_file);
	free(xor_check);
}

// Write dictionary to output file
char* save_dictionary(char* dictionary, FILE *output_file, unsigned char* xor_check) {
	char *eight_bits = (char *)malloc(9);
    unsigned char output_buffer[BUFFER_LIMIT];
    int output_buffer_length = 0;
    char *rest_of_the_dictionary = write_first_two_bytes(dictionary, output_file);
    while (strlen(rest_of_the_dictionary) >= 8) {
        get_eight_bits(rest_of_the_dictionary, eight_bits);
        output_buffer[output_buffer_length] = (unsigned char)strtol(eight_bits, NULL, 2);

        *xor_check ^= output_buffer[output_buffer_length];
        output_buffer_length++;

        memmove(rest_of_the_dictionary, rest_of_the_dictionary + 8, strlen(rest_of_the_dictionary) - 7);  // Move remaining bits forward

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
char* write_first_two_bytes(char* dictionary, FILE *output_file) {
    unsigned char output_buffer[2];
    int output_buffer_length = 0;
	char *eight_bits = (char *)malloc(9);

    if (strlen(dictionary) >= 16) {
        for (int i = 0; i < 2; i++) {
            get_eight_bits(dictionary, eight_bits);
            output_buffer[output_buffer_length++] = (unsigned char)strtol(eight_bits, NULL, 2);
			memmove(dictionary, dictionary + 8, strlen(dictionary) - 7);
        }
        fwrite(output_buffer, 1, output_buffer_length, output_file);
    }
    fflush(output_file);
	free(eight_bits);
    return dictionary;
}

void write_n_of_missing_bits(int n_of_missing_bits, FILE *output_file) {
    unsigned char second_byte[1];
	int read = fread(second_byte, 1, 1, output_file);
	if (read != 1) {
		printf("Something went wrong!\n");
	}

    char *binary_string = char_binary_code(second_byte[0], 8);
    char *binary_string_of_n_of_missing_bits = char_binary_code(n_of_missing_bits, 3);

    // Replace the first 3 bits of the original code with the last 3 bits of the nOfMissingBits code
    strncpy(binary_string + 5, binary_string_of_n_of_missing_bits, 3);

	int value = (int)strtol(binary_string, NULL, 2);
	unsigned char byte = (unsigned char)value;
	fwrite(&byte, 1, 1, output_file);
	free(binary_string);
	free(binary_string_of_n_of_missing_bits);
}

// Helper function to get the first 8 bits of a string
void get_eight_bits(char *input_buffer, char *eight_bits) {
    strncpy(eight_bits, input_buffer, 8);
    eight_bits[8] = '\0';
}


// Write xor_check value to the first byte
void write_xor_check(FILE *output_file, unsigned char* xor_check) {
	rewind(output_file);
	fwrite(xor_check, 1, 1, output_file);
}