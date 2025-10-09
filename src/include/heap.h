/**
 * @file heap.h
 * @brief Header file for the min-heap data structure.
 *
 * This file defines the structures and function prototypes for a min-heap
 * implementation used to build the Huffman tree efficiently.
 */

#ifndef _HEAP_H_
#define _HEAP_H_

#include <stdio.h>
#include <stdlib.h>

#include "node.h"

/**
 * @struct heap_t
 * @brief A min-heap data structure for storing Huffman tree nodes.
 */
typedef struct heap_t {
    struct node_t** array;  ///< Array of pointers to nodes.
    size_t size;            ///< The current allocated size of the array.
    size_t n_of_elems;      ///< The number of elements currently in the heap.
} heap_t;

/**
 * @brief Creates a new min-heap with a given initial size.
 * @param size The initial size of the heap's array.
 * @return A pointer to the newly created heap.
 */
heap_t* make_heap(size_t size);

/**
 * @brief Frees all memory associated with a heap, including its nodes.
 * @param heap The heap to be freed.
 */
void free_heap(heap_t* heap);

/**
 * @brief Inserts a node into the min-heap.
 * @param heap The heap to insert into.
 * @param node The node to be inserted.
 */
void insert(heap_t* heap, node_t* node);

/**
 * @brief Extracts the node with the minimum frequency from the heap.
 * @param heap The heap to extract from.
 * @return The node with the minimum frequency.
 */
node_t* extract_min_value(heap_t* heap);

/**
 * @brief Doubles the allocated size of the heap's array.
 * @param heap The heap to resize.
 */
void double_heap_size(heap_t* heap);

/**
 * @brief Performs the heapify-up operation to maintain the heap property.
 * @param heap The heap to operate on.
 * @param index The index of the element to heapify up.
 */
void heap_up(heap_t* heap, size_t index);

/**
 * @brief Performs the heapify-down operation to maintain the heap property.
 * @param heap The heap to operate on.
 */
void heap_down(heap_t* heap);

/**
 * @brief Swaps two nodes in the heap's array.
 * @param heap The heap containing the nodes.
 * @param first_index The index of the first node.
 * @param second_index The index of the second node.
 */
void swap(heap_t* heap, size_t first_index, size_t second_index);

#endif  // _HEAP_H_
