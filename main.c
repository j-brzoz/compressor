#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
#include "node.h"
#include "dictionary.h"

int main(int argc, char **argv){

	if(argc != 2) {
		printf("Usage: ./comp file");
		return 0;
	}
	
	// ---------- file ----------
	char* filename = argv[1];
	printf("File: %s\n", filename);
	FILE *file;
	file = fopen(filename, "rb");
	if (file == NULL){
        	fprintf(stderr,"Error reading from  %s\n",filename);
		return 1;
    	}
	
	// ---------- frequencies ----------
	int frequencies[256] = {0};	
	
	// ---------- read ----------
	unsigned char* buffer = malloc(16000 * sizeof* buffer);
	int length;

	while ((length = fread(buffer, sizeof* buffer, 2000, file)) != 0) {
		for(int i = 0; i < length; i++) {
			frequencies[buffer[i]] += 1;
		}
	}

	// ---------- clean up ----------
	fclose(file);
	free(buffer);
	
	// ---------- proccess ----------
	int unique_chars = 0;
	heap_t* heap = make_heap(8);
	for(int i = 0;  i < 256; i++) {
		if(frequencies[i] != 0) {
			unique_chars += 1; 
			unsigned short value[1] = {(unsigned short)i};
			insert(heap, make_node(value, frequencies[i], NULL, NULL));
		}
	}

	// ---------- results ----------
	for(int i = 0; i < 256; i++) {
		if (frequencies[i] != 0) {
			printf("%d -> %d\n", i, frequencies[i]);
		}
	}
	for(int i = 0; i < unique_chars; i++) {
		printf("%c ", *(heap->array[i]->value));
	}


	// ---------- make codes ----------
	node_t* huffman_tree = make_huffman_tree(heap);
	char* codes[256] = { NULL };
	generate_codes(codes, huffman_tree, "");
	for(int i = 0 ; i < 256; i++) {
		if(codes[i] != NULL) {
			printf("%d -> %s\n", i, codes[i]);
			free(codes[i]);
		}
	}

    char* dictionary = generate_dictionary(huffman_tree);
    printf("%s\n", dictionary);

	// ---------- more clean up ----------
	free_heap(heap);
	free_node(huffman_tree);
    free(dictionary);
	return 0;
}
