#include "../header/file_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

/*
@requires existing and valid file name with permissions to read
@assigns nothing
@ensures returns 1 if the file is a sobf file, otherwise returns 0
*/
int is_sobf_file(char* file_name) {
    FILE* fd;
    char buffer[4]; // "SOBF" is 4 bytes long
    size_t read_result;

    fd = fopen(file_name, "rb"); // "rb" for reading binary files
    if (!fd) {
        perror("fopen");
        return 0;
    }

    // Read exactly 4 bytes
    read_result = fread(buffer, 4, 1, fd);
    fclose(fd);

    if (read_result != 1) {
        return 0;
    }

    return strncmp(buffer, "SOBF", 4) == 0; // strncmp compares two strings for the size provided as 3rd argument
}

/*
@requires existing and valid SOBF file name with permissions to read, and existing file_data structure
@assigns content->c, content->v, content->codes_table, content->values_table
@ensures saves the file data to the structure
*/
void extract_file_data(char* file_name, file_data* content) {
	FILE* fd;
	size_t read_result;
	int fseek_result;
	int fscanf_result;

	// initialize content
	content -> c = 0;
	content -> v = 0;

	// open file for reading
	fd = fopen(file_name, "rb"); // "rb" for reading binary files
	if (!fd) {
		perror("fopen");
		exit(1);
	}

	// put the cursor at the 5th byte (after "SOBF\n")
	fseek_result = fseek(fd, 5, SEEK_SET);
	if (fseek_result != 0) {
		perror("fseek");
		fclose(fd);
		exit(1);
	}

    // read and save "c" and "v"
	fscanf_result = fscanf(fd, "%d %d", &(content->c), &(content->v));
	if (fscanf_result != 2) {
        fprintf(stderr, "fscanf: failure reading sizes\n");
        fclose(fd);
        exit(1);
    }
	if (content->c <= 0 || content->v < 0) { // content->v can be 0, because the values table can be of size 0
		printf("\nError: negative sizes c = %d and v = %d\n", content->c, content->v);
		fclose(fd);
		exit(1);
	}

	// move cursor by 1 byte to bypass '\n' after reading 'v'
	fseek_result = fseek(fd, 1L, SEEK_CUR);
	if (fseek_result != 0) {
		perror("fseek");
		fclose(fd);
		exit(1);
	}

	// allocate and save data to the codes table
	content -> codes_table = (int *)malloc(content->c * sizeof(int));
	if (!content->codes_table) {
        perror("malloc");
        exit(1);
    }	
	read_result = fread(content->codes_table, 4, content->c, fd); // 4 bytes = 32 bits like instructed
	if (read_result != (size_t)content->c) { // a cast used because content->c is integer and read_size is size_t type
		free(content -> codes_table);
		fclose(fd);
		exit(1);
	}

	// allocate and save data to the values table
	content -> values_table = (long int *)malloc(content->v * sizeof(long int));
	if (!content->values_table) {
        perror("malloc");
        exit(1);
    }	
	read_result = fread(content->values_table, 8, content->v, fd); // 8 bytes = 64 bits like instructed
	if (read_result != (size_t)content->v) { // a cast used because content->v is integer and read_size is size_t type
		free(content -> values_table);
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