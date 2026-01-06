#ifndef _FILE_READER_H
#define _FILE_READER_H

struct file_data {
	int c;
	int v;
	int* codes_table;
	long int* values_table;
};

typedef struct file_data file_data;

void extract_file_data(char*, file_data*);
void free_file_data(file_data*);

#endif