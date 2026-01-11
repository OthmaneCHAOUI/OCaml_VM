#ifndef _FILE_READER_H
#define _FILE_READER_H

// structure definition to store data from the file
struct file_data {
	int c;					// size of codes table
	int v;					// size of values table
	int* codes_table;		// code table data
	long int* values_table; // values table data
};

typedef struct file_data file_data;

int is_sobf_file(char*);
void extract_file_data(char*, file_data*);
void free_file_data(file_data*);

#endif