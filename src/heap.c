/**
 * @file heap.c
 * @brief Implementation of a min-heap data structure.
 *
 * This file provides the functions for creating, managing, and operating on a
 * min-heap. The heap is used to efficiently build the Huffman tree by always
 * providing the two nodes with the lowest frequencies.
 */

#include "heap.h"

/**
 * @brief Creates a new min-heap with a given initial size.
 * @param size The initial size of the heap's array.
 * @return A pointer to the newly created heap.
 */
heap_t* make_heap(size_t size) {
    heap_t* heap = malloc(sizeof(*heap));
    if (!heap) {
        fprintf(stderr, "Memory allocation failed for heap\n");
        exit(EXIT_FAILURE);
    }

    node_t** array = malloc(size * sizeof(*array));
    if (!array) {
        fprintf(stderr, "Memory allocation failed for heap array\n");
        free(heap);
        exit(EXIT_FAILURE);
    }

    heap->array = array;
    heap->size = size;
    heap->n_of_elems = 0;
    return heap;
}

/**
 * @brief Frees all memory associated with a heap, including its nodes.
 * @param heap The heap to be freed.
 */
void free_heap(heap_t* heap) {
    if (heap) {
        if (heap->array) {
            for (size_t i = 0; i < heap->n_of_elems; i++) {
                if (heap->array[i]) {
                    free_node(heap->array[i]);
                }
            }
            free(heap->array);
        }
        free(heap);
    }
}

/**
 * @brief Inserts a node into the min-heap.
 * @param heap The heap to insert into.
 * @param node The node to be inserted.
 */
void insert(heap_t* heap, node_t* node) {
    if (!heap || !node) {
        fprintf(stderr, "Error with inserting onto heap.\n");
        exit(EXIT_FAILURE);
    }

    if (heap->n_of_elems == heap->size) {
        double_heap_size(heap);
    }

    heap->array[heap->n_of_elems] = node;
    heap->n_of_elems += 1;
    heap_up(heap, heap->n_of_elems - 1);
}

/**
 * @brief Extracts the node with the minimum frequency from the heap.
 * @param heap The heap to extract from.
 * @return The node with the minimum frequency.
 */
node_t* extract_min_value(heap_t* heap) {
    if (!heap) {
        fprintf(stderr, "Error with extracting value from heap.\n");
        exit(EXIT_FAILURE);
    }

    if (heap->n_of_elems == 0) {
        fprintf(stderr, "Cannot extract from empty heap!\n");
        return NULL;
    }

    node_t* min_value = heap->array[0];
    heap->array[0] = heap->array[heap->n_of_elems - 1];
    heap->array[heap->n_of_elems - 1] = NULL;
    heap->n_of_elems -= 1;
    heap_down(heap);

    return min_value;
}

/**
 * @brief Doubles the allocated size of the heap's array.
 * @param heap The heap to resize.
 */
void double_heap_size(heap_t* heap) {
    if (!heap) {
        fprintf(stderr, "Error with incresing heap size.\n");
        exit(EXIT_FAILURE);
    }

    heap->size *= 2;
    node_t** new_array = realloc(heap->array, heap->size * sizeof **new_array);
    if (!new_array) {
        fprintf(stderr, "Error with incresing heap size.\n");
        exit(EXIT_FAILURE);
    }

    heap->array = new_array;
}

/**
 * @brief Performs the heapify-up operation to maintain the heap property.
 * @param heap The heap to operate on.
 * @param index The index of the element to heapify up.
 */
void heap_up(heap_t* heap, size_t index) {
    if (!heap) {
        fprintf(stderr, "Error with 'heap up' operation.\n");
        exit(EXIT_FAILURE);
    }

    size_t parent_index = (index - 1) / 2;
    while (index > 0 && heap->array[index]->frequency <
                            heap->array[parent_index]->frequency) {
        swap(heap, index, parent_index);
        index = parent_index;
        parent_index = (index - 1) / 2;
    }
}

/**
 * @brief Performs the heapify-down operation to maintain the heap property.
 * @param heap The heap to operate on.
 */
void heap_down(heap_t* heap) {
    if (!heap) {
        fprintf(stderr, "Error with 'heap down' operation.\n");
        exit(EXIT_FAILURE);
    }

    if (heap->n_of_elems == 0) return;

    size_t index = 0;
    size_t left_child_index = 2 * index + 1;
    size_t right_child_index = 2 * index + 2;
    size_t smallest_index = index;

    while (index <= heap->n_of_elems - 1) {
        if (left_child_index <= heap->n_of_elems - 1 &&
            heap->array[left_child_index]->frequency <
                heap->array[smallest_index]->frequency) {
            smallest_index = left_child_index;
        }
        if (right_child_index <= heap->n_of_elems - 1 &&
            heap->array[right_child_index]->frequency <
                heap->array[smallest_index]->frequency) {
            smallest_index = right_child_index;
        }
        if (smallest_index != index) {
            swap(heap, smallest_index, index);
            index = smallest_index;
            left_child_index = 2 * index + 1;
            right_child_index = 2 * index + 2;
        } else {
            break;
        }
    }
}

/**
 * @brief Swaps two nodes in the heap's array.
 * @param heap The heap containing the nodes.
 * @param first_index The index of the first node.
 * @param second_index The index of the second node.
 */
void swap(heap_t* heap, size_t first_index, size_t second_index) {
    if (!heap) {
        fprintf(stderr, "Error with swapping nodes.\n");
        exit(EXIT_FAILURE);
    }

    node_t* temp_node = heap->array[first_index];
    heap->array[first_index] = heap->array[second_index];
    heap->array[second_index] = temp_node;
}
