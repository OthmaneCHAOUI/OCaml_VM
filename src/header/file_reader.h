#ifndef _FILE_READER_H
#define _FILE_READER_H

// structure definition to store data from the file
struct file_data {
	int c;
	int v;
	int* codes_table;
	long int* values_table;
};

typedef struct file_data file_data;

int is_sobf_file(char*);
void extract_file_data(char*, file_data*);
void free_file_data(file_data*);

#endif