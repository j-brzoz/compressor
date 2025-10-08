#include "node.h"
#include "dictionary.h"
#include "heap.h"

// Make node with given parameters
node_t *make_node(const unsigned char *value, int frequency, node_t *left_child,
                  node_t *right_child) {
  node_t *node = malloc(sizeof(*node));
  if (!node) {
    fprintf(stderr, "Memory allocation failed for node\n");
    exit(EXIT_FAILURE);
  }

  if (value) {
    node->value = malloc(sizeof(*node->value));
    if (!node->value) {
      fprintf(stderr, "Memory allocation failed for node value\n");
      free(node);
      exit(EXIT_FAILURE);
    }
    *(node->value) = *value;
  } else {
    node->value = NULL;
  }

  node->frequency = frequency;
  node->left_child = left_child;
  node->right_child = right_child;
  return node;
}

// Free node structure
void free_node(node_t *node) {
  if (node != NULL) {
    if (node->left_child != NULL) {
      free_node(node->left_child);
    }
    if (node->right_child != NULL) {
      free_node(node->right_child);
    }
    if (node->value != NULL) {
      free(node->value);
    }
    free(node);
  }
}

// Make Huffman tree from heap
node_t *make_huffman_tree(heap_t *heap) {
  if (!heap) {
    fprintf(stderr, "Error with making Huffman tree.\n");
    exit(EXIT_FAILURE);
  }

  if (heap->n_of_elems == 1) {
    return extract_min_value(heap);
  }

  while (heap->n_of_elems > 1) {
    node_t *left_child = extract_min_value(heap);
    node_t *right_child = extract_min_value(heap);
    node_t *parent =
        make_node(NULL, left_child->frequency + right_child->frequency,
                  left_child, right_child);
    insert(heap, parent);
  }
  return extract_min_value(heap);
}

// Helper for debugging
void print_huffman_tree(const node_t *huffman_tree, char parent) {
  if (huffman_tree) {
    if (huffman_tree->value) {
      printf("%c %i %i\n", parent, huffman_tree->frequency,
             *(huffman_tree->value));
    } else {
      printf("%c NULL\n", parent);
    }
    print_huffman_tree(huffman_tree->left_child, 'l');
    print_huffman_tree(huffman_tree->right_child, 'r');
  }
}

// Append bit to binary string
char *add_bit_to_code(const char *base_code, char bit) {
  if (!base_code) {
    fprintf(stderr, "Error with adding bit to code.\n");
    exit(EXIT_FAILURE);
  }

  size_t length = strlen(base_code);
  // +2 for new bit and null terminator
  char *new_code = malloc((length + 2) * sizeof(*new_code));
  if (!new_code) {
    fprintf(stderr, "Memory allocation failed for new code\n");
    exit(EXIT_FAILURE);
  }

  memcpy(new_code, base_code, length);
  new_code[length] = bit;
  new_code[length + 1] = '\0';
  return new_code;
}

// Fill codes array from traversing Huffman tree
void generate_codes(const char **codes, const node_t *node, const char *code) {
  if (!node || !code || !codes)
    return;

  if (!node->left_child && !node->right_child) {
    codes[*(node->value)] = strdup(code);
    if (!codes[*(node->value)]) {
      fprintf(stderr, "Memory allocation failed for Huffman code\n");
      exit(EXIT_FAILURE);
    }
    return;
  }

  if (node->left_child) {
    char *left_code = add_bit_to_code(code, '0');
    generate_codes(codes, node->left_child, left_code);
    free(left_code);
  }

  if (node->right_child) {
    char *right_code = add_bit_to_code(code, '1');
    generate_codes(codes, node->right_child, right_code);
    free(right_code);
  }
}

// Make huffman tree from dictionary
node_t *make_huffman_tree_from_dict(dict_t *dictionary) {
  if (dictionary->index_helper >= (int)strlen(dictionary->dictionary)) {
    fprintf(stderr, "Error: Dictionary index out of bounds\n");
    return NULL;
  }

  char current_bit = dictionary->dictionary[dictionary->index_helper++];
  if (current_bit == '0') {
    return make_node(NULL, 0, make_huffman_tree_from_dict(dictionary),
                     make_huffman_tree_from_dict(dictionary));
  } else if (current_bit == '1') {
    char value_str[9]; // 8 bits + null terminator
    strncpy(value_str, dictionary->dictionary + dictionary->index_helper, 8);
    value_str[8] = '\0';

    unsigned char parsed_value = (unsigned char)strtol(value_str, NULL, 2);
    dictionary->index_helper += 8;

    return make_node(&parsed_value, 0, NULL, NULL);
  }

  fprintf(stderr, "Error: Invalid Huffman tree encoding\n");
  return NULL;
}
