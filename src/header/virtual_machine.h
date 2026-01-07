#ifndef _VIRTUAL_MACHINE_H
#define _VIRTUAL_MACHINE_H

#include "stack.h"

// the virtual machine's definition
struct VM {
	int index;
	long int acc;
	long int* globals;
	int global_size;
	int* codes;
	int codes_size;
	Stack stack;
	long int* atoms;
};

typedef struct VM VM;

void init_vm(VM*, long int*, int, int*, int);
void run_vm(VM*);

#endif