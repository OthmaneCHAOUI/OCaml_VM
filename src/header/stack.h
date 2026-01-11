#ifndef _STACK_H
#define _STACK_H

// structure definition for the stack
struct Stack {
    long int* data;     // table containing data
    int capacity;       // maximum capacity of the stack
    int size;           // current size of the stack
};

typedef struct Stack Stack;

int stack_is_empty(Stack);
int stack_is_full(Stack);
void resize_stack(Stack*, int);
void init_stack(Stack*, int);
void stack_push(Stack*, long int);
long int stack_pop(Stack*);
void free_stack(Stack*);
void print_stack(Stack);
long int stack_seek(Stack, int);
long int stack_top(Stack);

#endif
