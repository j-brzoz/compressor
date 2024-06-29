#include "heap.h"

heap_t* make_heap(int size) {
	heap_t* heap = malloc(sizeof* heap);
	node_t** array = malloc(size * sizeof** array);
	
	heap->array = array;
	heap->size = size;
	heap->n_of_elems = 0;
	return heap;
}

void free_heap(heap_t* heap) {
	if (heap != NULL) {
		if (heap->array != NULL) {
			for(int i = 0; i < heap->n_of_elems; i++) {
				if (heap->array[i] != NULL) {
					free(heap->array[i]);
				}
			}
			free(heap->array);
		}
		free(heap);
	}
}

void insert(heap_t* heap, node_t* node) {
	if(heap->size == heap->n_of_elems) {
		double_heap_size(heap);
	}
	heap->array[heap->n_of_elems] = node;
	heap->n_of_elems += 1;
	heap_up(heap, heap->n_of_elems - 1);
}

node_t* extract_min_value(heap_t* heap) {
	if (heap->n_of_elems == 0) {
		fprintf(stderr, "Cannot extract from empty heap!");
		return NULL;
	}	
	
	node_t* min_value = heap->array[0];
	heap->array[0] = heap->array[heap->n_of_elems - 1];
	heap->n_of_elems -= 1;
	heap_down(heap);

	return min_value;
}

void double_heap_size(heap_t* heap) {
	heap->size *= 2;
	node_t** new_array = realloc(heap->array, heap->size * sizeof** new_array);
	heap->array = new_array;
}

void heap_up(heap_t* heap, int index) {
	int parent_index = (index - 1) / 2;
	while(1) {
		if(heap->array[index]->frequency < heap->array[parent_index]->frequency) {
			swap(heap, index, parent_index);
		}
		if(parent_index == 0) {
			break;
		}
		index = parent_index;
		parent_index = (index - 1 ) / 2;
	}
}

void heap_down(heap_t* heap) {
	int index = 0;
	int left_child_index = 2 * index + 1;
	int right_child_index = 2 * index + 2;
	int smallest_index = index;

	while(index <= heap->n_of_elems - 1) {
		if(left_child_index <= heap->n_of_elems - 1 && heap->array[left_child_index] < heap->array[smallest_index]) {
			smallest_index = left_child_index;
		}	
		if(right_child_index <= heap->n_of_elems - 1 && heap->array[right_child_index] < heap->array[smallest_index]) {
			smallest_index = right_child_index;
		}
		if(smallest_index != index) {
			swap(heap, smallest_index, index);
			index = smallest_index;
			left_child_index = 2 * index + 1;
			right_child_index = 2 * index + 2;
		} else {
			break;
		}
	}
}

void swap(heap_t* heap, int first_index, int second_index) {
	node_t* temp_node = heap->array[first_index];
       	heap->array[first_index] = heap->array[second_index];
	heap->array[second_index] = temp_node;
}

