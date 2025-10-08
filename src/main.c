#include "decoder.h"
#include "dictionary.h"
#include "encoder.h"
#include "heap.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s mode original_file output_file\n", argv[0]);
    fprintf(stderr, "   mode: 0 -> compression, 1 -> decompression\n");
    return EXIT_FAILURE;
  }

  // ---------- file ----------
  const char *mode_str = argv[1];
  const char *original_filename = argv[2];
  const char *output_filename = argv[3];

  FILE *original_file = fopen(original_filename, "rb+");
  if (original_file == NULL) {
    fprintf(stderr, "Error: Could not open input file '%s' for reading.\n",
            original_filename);
    return EXIT_FAILURE;
  }

  FILE *output_file = fopen(output_filename, "wb+");
  if (output_file == NULL) {
    fprintf(stderr, "Error: Could not open output file '%s' for writing.\n",
            output_filename);
    fclose(original_file);
    return EXIT_FAILURE;
  }

  if (strcmp(mode_str, "1") == 0) { // decompression
    // ---------- get dictionary from the file ----------
    dict_t *dictionary = get_dictionary_from_file(original_filename);
    if (dictionary == NULL) {
      fprintf(stderr, "Error: Failed to read dictionary from file '%s'.\n",
              original_filename);
      fclose(original_file);
      fclose(output_file);
      return EXIT_FAILURE;
    }

    // ---------- make the Huffman tree from the dictionary ----------
    node_t *huffman_tree = make_huffman_tree_from_dict(dictionary);
    if (huffman_tree == NULL) {
      fprintf(stderr, "Error: Failed to build Huffman tree from dictionary.\n");
      free_dictionary(dictionary);
      fclose(original_file);
      fclose(output_file);
      return EXIT_FAILURE;
    }

    // ---------- decompress the file ----------
    decompress(original_file, output_file, dictionary, huffman_tree);

    // ---------- clean up ----------
    free_dictionary(dictionary);
    free_node(huffman_tree);
  } else if (strcmp(argv[1], "0") == 0) { // compression
    // ---------- get byte frequencies ----------
    size_t frequencies[256] = {0};
    const size_t buffer_size = 2048;
    unsigned char *buffer = malloc(buffer_size * sizeof *buffer);
    size_t length;

    while ((length = fread(buffer, sizeof *buffer, buffer_size,
                           original_file)) != 0) {
      for (size_t i = 0; i < length; i++) {
        frequencies[buffer[i]]++;
      }
    }
    free(buffer);

    // ---------- make a heap ----------
    heap_t *heap = make_heap(8);
    if (heap == NULL) {
      fprintf(stderr, "Error: Failed to create heap.\n");
      fclose(original_file);
      fclose(output_file);
      return EXIT_FAILURE;
    }

    // ---------- insert nodes ----------
    for (size_t i = 0; i < 256; i++) {
      if (frequencies[i] != 0) {
        const unsigned char value[1] = {(unsigned char)i};
        insert(heap, make_node(value, frequencies[i], NULL, NULL));
      }
    }

    // ---------- make codes ----------
    node_t *huffman_tree = make_huffman_tree(heap);
    if (huffman_tree == NULL) {
      fprintf(stderr, "Error: Failed to build Huffman tree.\n");
      free_heap(heap);
      fclose(original_file);
      fclose(output_file);
      return EXIT_FAILURE;
    }

    const char *codes[256] = {NULL};
    generate_codes(codes, huffman_tree, "");

    // ---------- make the dictionary ----------
    char *dictionary = generate_dictionary(huffman_tree);
    if (dictionary == NULL) {
      fprintf(stderr, "Error: Failed to generate dictionary.\n");
      for (int i = 0; i < 256; i++) {
        if (codes[i] != NULL) {
          free((void *)codes[i]);
        }
      }
      free_heap(heap);
      free_node(huffman_tree);
      fclose(original_file);
      fclose(output_file);
      return EXIT_FAILURE;
    }

    // ---------- compress the file ----------
    rewind(original_file);
    compress(dictionary, codes, original_file, output_file);

    // ---------- clean up ----------
    for (int i = 0; i < 256; i++) {
      if (codes[i] != NULL) {
        free((void *)codes[i]);
      }
    }
    free_heap(heap);
    free_node(huffman_tree);
  } else {
    fprintf(stderr,
            "Error: Unknown mode '%s'. "
            "Valid modes are 0 (compression) and 1 (decompression).\n",
            mode_str);
    fclose(original_file);
    fclose(output_file);
    return EXIT_FAILURE;
  }
  return 0;
}
