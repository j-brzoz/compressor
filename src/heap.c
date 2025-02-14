#include "./includes/heap.h"


// Make heap with given initial size
heap_t *
make_heap(
	size_t size
) {
    heap_t *heap = malloc(sizeof(*heap));
    if (!heap) {
        fprintf(stderr, "Memory allocation failed for heap\n");
        exit(EXIT_FAILURE);
    }

    node_t **array = malloc(size * sizeof(*array));
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


// Free heap structure
void
free_heap(
	heap_t *heap
) {
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


// Insert node to the heap
void
insert(
	heap_t *heap,
	node_t *node
) {
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


// Get the top value and fix the heap after
node_t *
extract_min_value(
	heap_t *heap
) {
	if (!heap) {
        fprintf(stderr, "Error with extracting value from heap.\n");
        exit(EXIT_FAILURE);
    }

    if (heap->n_of_elems == 0) {
        fprintf(stderr, "Cannot extract from empty heap!\n");
        return NULL;
    }

    node_t *min_value = heap->array[0];
    heap->array[0] = heap->array[heap->n_of_elems - 1];
    heap->array[heap->n_of_elems - 1] = NULL;
    heap->n_of_elems -= 1;
    heap_down(heap);

    return min_value;
}


// Double heap size
void
double_heap_size(
	heap_t *heap
) {
	if (!heap) {
        fprintf(stderr, "Error with incresing heap size.\n");
        exit(EXIT_FAILURE);
    }

	heap->size *= 2;
	node_t **new_array = realloc(heap->array, heap->size * sizeof **new_array);
	if (!heap) {
        fprintf(stderr, "Error with incresing heap size.\n");
        exit(EXIT_FAILURE);
    }

	heap->array = new_array;
}


// Basic heap operation
void
heap_up(
	heap_t *heap,
	size_t index
) {
	if (!heap) {
        fprintf(stderr, "Error with 'heap up' operation.\n");
        exit(EXIT_FAILURE);
    }

    size_t parent_index = (index - 1) / 2;
    while (index > 0 &&
		heap->array[index]->frequency < heap->array[parent_index]->frequency) {
        swap(heap, index, parent_index);
        index = parent_index;
        parent_index = (index - 1) / 2;
    }
}


// Basic heap operation
void
heap_down(
	heap_t *heap
) {
	if (!heap) {
        fprintf(stderr, "Error with 'heap down' operation.\n");
        exit(EXIT_FAILURE);
    }

	if (heap->n_of_elems == 0) return;

	size_t index = 0;
	size_t left_child_index = 2 * index + 1;
	size_t right_child_index = 2 * index + 2;
	size_t smallest_index = index;

	while(index <= heap->n_of_elems - 1) {
		if(left_child_index <= heap->n_of_elems - 1 &&
			heap->array[left_child_index]->frequency <
				heap->array[smallest_index]->frequency) {
			smallest_index = left_child_index;
		}
		if(right_child_index <= heap->n_of_elems - 1 &&
			heap->array[right_child_index]->frequency <
				heap->array[smallest_index]->frequency) {
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


// Swap two nodes
void
swap(
	heap_t *heap,
	size_t first_index,
	size_t second_index
) {
	if (!heap) {
        fprintf(stderr, "Error with swapping nodes.\n");
        exit(EXIT_FAILURE);
    }

	node_t *temp_node = heap->array[first_index];
       	heap->array[first_index] = heap->array[second_index];
	heap->array[second_index] = temp_node;
}
