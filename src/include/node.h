/**
 * @file node.h
 * @brief Header file for Huffman tree node operations.
 *
 * This file defines the node structure for the Huffman tree and declares
 * functions for creating, managing, and traversing these nodes.
 */

#ifndef _NODE_H_
#define _NODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declaration
struct heap_t;
typedef struct heap_t heap_t;
struct dict_t;
typedef struct dict_t dict_t;

/**
 * @struct node_t
 * @brief Represents a node in the Huffman tree.
 */
typedef struct node_t {
    unsigned char* value;  ///< The character value (for leaf nodes). NULL for
                           ///< internal nodes.
    int frequency;  ///< The frequency of the character or sum of frequencies of
                    ///< children.
    struct node_t* left_child;   ///< Pointer to the left child node.
    struct node_t* right_child;  ///< Pointer to the right child node.
} node_t;

/**
 * @brief Creates a new node for the Huffman tree.
 * @param value The character value for the node (NULL for internal nodes).
 * @param frequency The frequency of the character.
 * @param left_child Pointer to the left child.
 * @param right_child Pointer to the right child.
 * @return A pointer to the newly created node.
 */
node_t* make_node(const unsigned char* value, int frequency, node_t* left_child,
                  node_t* right_child);

/**
 * @brief Recursively frees a node and all its descendants.
 * @param node The root node of the tree/subtree to free.
 */
void free_node(node_t* node);

/**
 * @brief Constructs a Huffman tree from a min-heap of nodes.
 * @param heap The min-heap containing initial leaf nodes.
 * @return The root node of the constructed Huffman tree.
 */
node_t* make_huffman_tree(heap_t* heap);

/**
 * @brief A helper function for debugging that prints the Huffman tree
 * structure.
 * @param huffman_tree The root of the tree to print.
 * @param parent A character indicating if it's a left ('l') or right ('r')
 * child.
 */
void print_huffman_tree(const node_t* huffman_tree, char parent);

/**
 * @brief Appends a bit ('0' or '1') to a base binary code string.
 * @param base_code The string to append to.
 * @param bit The character ('0' or '1') to append.
 * @return A new dynamically allocated string with the bit appended.
 */
char* add_bit_to_code(const char* base_code, char bit);

/**
 * @brief Recursively traverses the Huffman tree to generate binary codes for
 * each character.
 * @param codes An array to store the generated codes, indexed by character
 * value.
 * @param node The current node in the traversal.
 * @param code The binary code generated so far.
 */
void generate_codes(const char** codes, const node_t* node, const char* code);

/**
 * @brief Reconstructs the Huffman tree from its string representation
 * (dictionary).
 * @param dictionary The dictionary structure containing the string and parsing
 * state.
 * @return The root node of the reconstructed Huffman tree.
 */
node_t* make_huffman_tree_from_dict(dict_t* dictionary);

#endif  // _NODE_H_
