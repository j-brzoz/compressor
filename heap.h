#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include "node.h"

typedef struct heap_t{
	node_t** array;
	int size;
	int n_of_elems;
} heap_t;

heap_t* make_heap(int size);
void free_heap(heap_t* heap);
void insert(heap_t* heap, node_t* node);
node_t* extract_min_value(heap_t* heap);
void double_heap_size(heap_t* heap);
void heap_up(heap_t* heap, int index);
void heap_down(heap_t* heap);
void swap(heap_t* heap, int first_index, int second_index);


#endif
