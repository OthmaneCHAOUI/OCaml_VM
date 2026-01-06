#include "../header/file_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

/*
@requires existing and valid SOBF file name with permissions to read, and existing file_data structure
@assigns content->c, content->v, content->codes_table, content->values_table
@ensures saves the file data to the structure
*/
void extract_file_data(char* file_name, file_data* content) {
	FILE* fd;
	char buffer[4];
	size_t read_result;
	// int fseek_result;

	// open file for reading
	fd = fopen(file_name, "rb"); // "rb" for reading binary files
	if (!fd) {
		perror("fopen");
		exit(1);
	}

	// check if file is SOBF file
    if (fread(buffer, 1, 4, fd) != 4 || strncmp(buffer, "SOBF", 4) != 0) {
        fclose(fd);
        exit(EXIT_FAILURE);
    }

	// put the cursor at the 5th byte (after "SOBF\n")
/*	fseek_result = fseek(fd, 5, SEEK_SET);
	if (fseek_result != 0) {
		perror("fseek");
		fclose(fd);
		exit(1);
	}
*/

	// read and save "c" the size of the table of codes
	read_result = fread(&(content->c), sizeof(int), 1, fd);
	if (read_result != 1) {
		fclose(fd);
		exit(1);
	}

	// read and save "v" the size of the table of values
	read_result = fread(&(content->v), sizeof(int), 1, fd);
	if (read_result != 1) {
		fclose(fd);
		exit(1);
	}

	if (content->c <= 0 || content->v <= 0) {
		printf("\nerror: negative sizes c and v");
		fclose(fd);
		exit(1);
	}

	// allocate and save data to the codes table
	content -> codes_table = (int *)malloc(content->c * sizeof(int));
	// fseek(fd, 8, SEEK_SET); // move cursor to the third line
	read_result = fread(content->codes_table, sizeof(int), content->c, fd);
	if (read_result != (size_t)content->c) { // a cast because content->c is integer and read_size is size_t type
		fclose(fd);
		exit(1);
	}

	// allocate and save data to the values table
	content -> values_table = (long int *)malloc(content->v * sizeof(long int));
	// fseek(fd, 1, SEEK_CUR); // move cursor by 1 byte(to pass the "\n")
	read_result = fread(content->values_table, sizeof(long int), content->v, fd);
	if (read_result != (size_t)content->v) { // a cast because content->v is integer and read_size is size_t type
		fclose(fd);
		exit(1);
	}

	fclose(fd);
}

/*
@requires a not empty structure
@assigns nothing
@ensures frees the allocated space by extract_file_data function
*/
void free_file_data(file_data *content) {
    free(content->codes_table);
    free(content->values_table);
}