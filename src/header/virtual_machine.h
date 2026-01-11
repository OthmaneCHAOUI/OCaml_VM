#ifndef _VIRTUAL_MACHINE_H
#define _VIRTUAL_MACHINE_H

#include "stack.h" // for the Stack in the VM structure, and we are going to use its functions in run_vm function

// the virtual machine's definition to store the VM's state data
struct VM {
	int index;				// current index in codes table
	long int acc;			// accumulator
	int globals_size;		// globals's size
	int codes_size;			// size of codes table
	long int* globals;		// global values table
	int* codes;				// codes table
	Stack stack;			// stack
	long int* atoms;		// atoms table
};

typedef struct VM VM;

void init_vm(VM*, long int*, int, int*, int);
void run_vm(VM*);
void print_machine_state(VM);
void free_vm(VM*);

#endif