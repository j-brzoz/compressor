#ifndef SRC_INCLUDES_HEAP_H_
#define SRC_INCLUDES_HEAP_H_

#include "./node.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct heap_t {
  struct node_t **array;
  size_t size;
  size_t n_of_elems;
} heap_t;

// Make heap with given initial size
heap_t *make_heap(size_t size);

// Free heap structure
void free_heap(heap_t *heap);

// Insert node to the heap
void insert(heap_t *heap, node_t *node);

// Get the top value and fix the heap after
node_t *extract_min_value(heap_t *heap);

// Double heap size
void double_heap_size(heap_t *heap);

// Basic heap operation
void heap_up(heap_t *heap, size_t index);

// Basic heap operation
void heap_down(heap_t *heap);

// Swap two nodes
void swap(heap_t *heap, size_t first_index, size_t second_index);

#endif // SRC_INCLUDES_HEAP_H_
