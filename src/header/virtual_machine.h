#ifndef _FILE_READER_H
#define _FILE_READER_H

#include "stack.h"

// the virtual machine's definition
struct VM {
	int index;
	int acc;
	long int* global_values;
	int global_values_size;
	int* codes;
	int codes_size;
	Stack* s;
};

typedef struct VM VM;



#endif