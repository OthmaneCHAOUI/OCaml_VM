#include <stdio.h>
#include <stdlib.h>
#include "../header/stack.h"

/*
@requires stack s
@assigns nothing
@ensures returns 0 if s is empty, otherwise returns 1
*/
int stack_is_empty(Stack s) {
	return s.size == 0;
}

/*
@requires not empty stack s
@assigns nothing
@ensures returns 0 if s is full, otherwise returns 1
*/
int stack_is_full(Stack s) {
	return s.size == s.capacity;
}

/*
@requires not empty stack s and integer new_capacity
@assigns s->capacity
@ensures changes the stack's capacity to the new capacity; s->capacity = new_capacity
*/
void resize_stack(Stack *s, int new_capacity) {
    long int *new_data = (long int*)malloc(new_capacity * sizeof(long int));
    if (!new_data) {
 	    perror("malloc");
    	exit(EXIT_FAILURE);
	}

    for (int i = 0; i < s->size; i++)
        new_data[i] = s->data[i];

    free(s->data);
    s->data = new_data;
    s->capacity = new_capacity;
}

/*
@requires empty stack s and integer capacity > 0
@assigns s->size, s->capacity, s->data
@ensures initializes the stack; s->size = 0, s->capacity = capacity, s->data is allocated
*/
void init_stack(Stack* s, int capacity) {
	s -> data = (long int*)malloc(capacity * sizeof(long int));

	if (capacity <= 0)
	    capacity = 1;

	s -> capacity = capacity;
	s -> size = 0;
}

/*
@requires stack s, long int value
@assigns s->data, s->size
@ensures pushs 'value' to the stack; 'value' is at the top
*/
void stack_push(Stack* s, long int value) {
	if (stack_is_full(*s)) {
		resize_stack(s, 2 * s -> capacity);
	}

	s -> data[s->size] = value;
	s -> size++;
}

/*
@requires not empty stack s
@assigns s->data, s->size
@ensures pops the size value off the stack; s->size--, s->data has as the new top the value beneath the old top
*/
long int stack_pop(Stack* s) {
    if (stack_is_empty(*s)) {
        fprintf(stderr, "stack_pop: empty stack\n");
        exit(EXIT_FAILURE);
    }

    s->size--;
    return s->data[s->size];
}

/*
@requires not empty stack s
@assigns s->data
@ensures frees the alocated space for s->data
*/
void free_stack(Stack* s) {
	free(s -> data);
}

/*
@requires not empty stack s
@assigns nothing
@ensures prints the stack
*/
void print_stack(Stack s) {
	for (int i = s.size - 1; i >= 0; i--) {
		printf("%4ld", s.data[i]);
	}
	printf("\n");
}

/*
@requires not empty stack s and integer 0 < depth < s.size
@assigns nothing
@ensures returns element of s in a specific depth starting from 0 for the top element
*/
long int stack_seek(Stack s, int depth) {
    if (depth < 0 || depth >= s.size) {
        fprintf(stderr, "stack_peek: invalid depth\n");
        exit(EXIT_FAILURE);
    }

	return s.data[s.size - depth - 1];
}

/*
@requires not empty stack s
@assigns nothing
@ensures returns the top element of the stack
*/
long int stack_top(Stack s) {
    if (stack_is_empty(s)) {
        fprintf(stderr, "stack_top: empty stack\n");
        exit(EXIT_FAILURE);
    }
    return s.data[s.size - 1];
}
