#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/file_reader.h"
#include "../header/virtual_machine.h"
// no need to include stack.h, because it is already included in virtual_machine.h

int main(int argc, char *argv[]) {
    // check if number of arguments is 2 or more
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <bytecode_file> [--print-end-machine]\n", argv[0]);
        return 1;
    }

    char *file_name = argv[1];

    // check if file is a valid sobf file
    if (!is_sobf_file(file_name)) {
        fprintf(stderr, "Error: '%s' is not a valid SOBF file\n", file_name);
        return EXIT_FAILURE;
    }

    file_data prog;

    // extract data from sobf file
    extract_file_data(file_name, &prog);

    VM vm;

    // save data to the vm
    init_vm(&vm, prog.values_table, prog.v, prog.codes_table, prog.c);

    // run vm
    run_vm(&vm);

    // print machine's state
    if (argc >= 3 && strcmp(argv[2], "--print-end-machine") == 0) {
        print_machine_state(vm);
    }

    // free allocated spaces
    free_file_data(&prog);
    free_vm(&vm);

    return 0;
}