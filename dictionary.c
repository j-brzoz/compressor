#include "dictionary.h"

#define BUFFER_LIMIT 8192

char* generate_dictionary(node_t* huffman_tree) {
    char* dictionary = (char*)malloc(12);  // "00000000" + "000" + null terminator
    strcpy(dictionary, "00000000" "000"); // xor value + number of added bits
    
    node_t* node_stack[256];
    int idx = 0;

    node_stack[idx++] = huffman_tree;

    while (idx > 0) {
        node_t* node = node_stack[--idx];

        if (node->value == NULL) {  
            dictionary = realloc(dictionary, strlen(dictionary) + 2);
            strcat(dictionary, "0");

            node_stack[idx++] = node->left_child;
            node_stack[idx++] = node->right_child;
        } else {
            dictionary = realloc(dictionary, strlen(dictionary) + 2);
            strcat(dictionary, "1");

            char* binary_code = char_binary_code(*(node->value), 8);
            dictionary = realloc(dictionary, strlen(dictionary) + strlen(binary_code) + 1);
            strcat(dictionary, binary_code);
            free(binary_code);
        }
    }
    return dictionary;
}

char* char_binary_code(unsigned char value, int bits) {
    char* binary_code = malloc((bits + 1) * sizeof *binary_code);
    binary_code[bits] = '\0';

    for (int i = bits - 1; i >= 0; --i) {
        binary_code[i] = (value & 1) ? '1' : '0';
        value >>= 1;
    }
    return binary_code;
}

dict_t* create_dict() {
    dict_t* dictionary = malloc(sizeof(dict_t));
    dictionary->n_of_leaves = 0;
    dictionary->n_of_non_leaves = 0;
    dictionary->n_of_added_bits = 0;
    dictionary->dictionary = NULL;
    dictionary->n_of_read_bytes = 0;
	dictionary->index_helper = 0;
    return dictionary;
}

void free_dictionary(dict_t* dictionary) {
    if (dictionary) {
        if (dictionary->dictionary) {
            free(dictionary->dictionary);
		}
        free(dictionary);
    }
}

char* analyze_dictionary_prefix(dict_t* dictionary, char* filename) {

	FILE* file = fopen(filename, "rb+");
	int read;
	unsigned char byte_from_input_file[2];
	char* remaining = NULL;

	dictionary->n_of_read_bytes += 2;
	read = fread(byte_from_input_file, 1, 2, file);
	fclose(file);


	check_if_valid_file(filename);

	file = fopen(filename, "rb+");
	fseek(file, 2, SEEK_SET);



	if (read > 0) {
		// get number of added bits
		char* binary_code = char_binary_code(byte_from_input_file[1], 8);

		// Extract the first 3 bits
		char temp[4] = {0};
		strncpy(temp, binary_code, 3);
		dictionary->n_of_added_bits = (int) strtol(temp, NULL, 2);

		remaining = strdup(binary_code + 3);
		free(binary_code);
	}
    fclose(file);
    return remaining;
}

int get_dict_stats(dict_t* dictionary, int index) {
    while (dictionary->n_of_leaves != dictionary->n_of_non_leaves + 1 && index < (int)strlen(dictionary->dictionary)) {
        if (dictionary->dictionary[index] == '0') {
            index++;
            dictionary->n_of_non_leaves++;
        } else if (dictionary->dictionary[index] == '1') {
            index += 1 + 8; // char length = 8
            dictionary->n_of_leaves++;
        }
    }
    return index;
}

dict_t* get_dictionary_from_file(char* filename) {
	int prefix_length = 2;
	int read;
    unsigned char byte_from_input_file[1];

	dict_t* dictionary = create_dict();

    dictionary->dictionary = analyze_dictionary_prefix(dictionary, filename);

    int dictionary_index = 0;
    dictionary_index = get_dict_stats(dictionary, dictionary_index);


    // skip the prefix bytes
	FILE* file = fopen(filename, "rb+");
    fseek(file, prefix_length, SEEK_SET);

    while (dictionary->n_of_leaves != dictionary->n_of_non_leaves + 1 && (read = fread(byte_from_input_file, 1, 1, file)) > 0) {
        dictionary->n_of_read_bytes++;
        char *bin_code = char_binary_code(byte_from_input_file[0], 8);

        dictionary->dictionary = realloc(dictionary->dictionary, strlen(dictionary->dictionary) + strlen(bin_code) + 1);
        strcat(dictionary->dictionary, bin_code);
        free(bin_code);

        dictionary_index = get_dict_stats(dictionary, dictionary_index);
    }
	

    if (dictionary_index > (int)strlen(dictionary->dictionary)) {
		read = fread(byte_from_input_file, 1, 1, file);
        if (read > 0) {
            dictionary->n_of_read_bytes++;
            char *bin_code = char_binary_code(byte_from_input_file[0], 8);

            dictionary->dictionary = realloc(dictionary->dictionary, strlen(dictionary->dictionary) + strlen(bin_code) + 1);
            strcat(dictionary->dictionary, bin_code);
            free(bin_code);
        }
    }
    fclose(file);
	return dictionary;
}

void check_if_valid_file(char* filename) {
	FILE* file = fopen(filename, "rb");


	int read;
	unsigned char first_two_bytes[2];

	read = fread(first_two_bytes, 1, 2, file);

	unsigned char xor = first_two_bytes[0];


	unsigned char bytes_from_input_file[BUFFER_LIMIT];
	while ((read = fread(bytes_from_input_file, 1, BUFFER_LIMIT, file)) > 0) {
		for (int i = 0; i < read; i++) {
			xor ^= bytes_from_input_file[i];
		}
	}
	fclose(file);

	if (xor != 'a') {
		fprintf(stderr, "Broken file: %s\n", filename);
		exit(1);
	}
}