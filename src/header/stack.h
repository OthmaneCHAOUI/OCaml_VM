#ifndef _STACK_H
#define _STACK_H

struct Stack {
    long int *data;
    int capacity;
    int size;
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

#endif
