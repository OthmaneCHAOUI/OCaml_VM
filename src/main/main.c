#include <stdio.h>
#include <stdlib.h>
#include "../header/file_reader.h"
#include "../header/virtual_machine.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return EXIT_FAILURE;
    }

/* 
    char *filename = argv[1];

    if (!is_sobf_file(filename)) {
        fprintf(stderr, "Error: '%s' is not a valid SOBF file\n", filename);
        return EXIT_FAILURE;
    }

    file_data prog;

    extract_file_data(filename, &prog);

    VM vm;

    vm_init(
        &vm,
        prog.codes_table, prog.c,
        prog.values_table, prog.v
    );

    vm_run(&vm);

    printf("Program finished.\n");
    printf("ACC = %ld\n", vm.acc);

    vm_free(&vm);
    free(prog.codes_table);
    free(prog.values_table);
*/
    return EXIT_SUCCESS;
}