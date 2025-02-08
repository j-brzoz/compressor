#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
#include "node.h"
#include "dictionary.h"
#include "encoder.h"
#include "decoder.h"

int main(int argc, char **argv){

	if(argc != 3) {
		printf("Usage: ./comp mode file\n");
		return 0;
	}
	
	// ---------- file ----------
	char* filename = argv[2];
	printf("File: %s\n", filename);

	if (strcmp(argv[1], "1") == 0) {
		dict_t* dictionary = get_dictionary_from_file(filename);
		node_t* huffman_tree = make_huffman_tree_from_dict(dictionary);
		// print_huffman_tree(huffman_tree, 's');
		FILE *file;
		file = fopen(filename, "rb+");
		FILE *output_file;
		output_file = fopen("decompressed.png", "wb+");
		decompress(file, output_file, dictionary, huffman_tree);
		free_dictionary(dictionary);
		free_node(huffman_tree);
	} else if (strcmp(argv[1], "0") == 0) {
		FILE *file;
		file = fopen(filename, "rb+");
		if (file == NULL){
			fprintf(stderr,"Error reading from  %s\n",filename);
			return 1;
		}
		// ---------- frequencies ----------
		int frequencies[256] = {0};	
		
		// ---------- read ----------
		unsigned char* buffer = malloc(2048*8 * sizeof* buffer);
		int length;

		while ((length = fread(buffer, sizeof* buffer, 2048, file)) != 0) {
			for(int i = 0; i < length; i++) {
				frequencies[buffer[i]] += 1;
			}
		}

		// ---------- clean up ----------
		free(buffer);
		
		// ---------- proccess ----------
		int unique_chars = 0;
		heap_t* heap = make_heap(8);
		for(int i = 0;  i < 256; i++) {
			if(frequencies[i] != 0) {
				unique_chars += 1; 
				unsigned char value[1] = {(unsigned char)i};
				insert(heap, make_node(value, frequencies[i], NULL, NULL));
			}
		}

		// ---------- results ----------
		// for(int i = 0; i < 256; i++) {
		// 	if (frequencies[i] != 0) {
		// 		printf("%d -> %d\n", i, frequencies[i]);
		// 	}
		// }
		// for(int i = 0; i < unique_chars; i++) {
		// 	printf("%c ", *(heap->array[i]->value));
		// }


		// ---------- make codes ----------
		node_t* huffman_tree = make_huffman_tree(heap);
		// print_huffman_tree(huffman_tree, 's');
		char* codes[256] = { NULL };
		generate_codes(codes, huffman_tree, "");
		// for(int i = 0; i < 256; i++) {
		// 	if (codes[i])
		// 		printf("%d %s\n", i , codes[i]);
		// }

		char* dictionary = generate_dictionary(huffman_tree);
		// printf("\n %s\n", dictionary);

		FILE *out;
		out = fopen("compressed.comp", "wb+");
		rewind(file);
		compress(dictionary, codes, file, out);
		
		// ---------- more clean up ----------
		for(int i = 0 ; i < 256; i++) {
			if(codes[i] != NULL) {
				free(codes[i]);
			}
		}
		
		free_heap(heap);
		free_node(huffman_tree);
	} else {
		printf("Unknown mode\n");
	}
	return 0;
}
