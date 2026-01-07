#ifndef _VIRTUAL_MACHINE_H
#define _VIRTUAL_MACHINE_H

#include "stack.h" // for the Stack in the VM, and we are going tu use their functions in run_vm function

// the virtual machine's definition to store the VM's state data
struct VM {
	int index;
	long int acc;
	long int* globals;
	int globals_size;
	int* codes;
	int codes_size;
	Stack stack;
	long int* atoms;
};

typedef struct VM VM;

void init_vm(VM*, long int*, int, int*, int);
void run_vm(VM*);
void print_machine_state(VM);

#endif