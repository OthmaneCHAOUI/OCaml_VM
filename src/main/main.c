#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/file_reader.h"
#include "../header/stack.h"
#include "../header/virtual_machine.h"

void print_machine_state(VM vm) {
    printf("Index: %d\n", vm.index);
    printf("Accumulator: %ld\n", vm.acc);
    
    printf("Stack:\n");
    print_stack(vm.stack);

    printf("Global:\n");
    // global_size doit être stocké dans votre structure VM lors du chargement
    for (int i = 0; i < vm.global_size; i++) {
        printf("%d %ld\n", i, vm.globals[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <bytecode_file> [--print-end-machine]\n", argv[0]);
        return 1;
    }

    char *file_name = argv[1];

    if (!is_sobf_file(file_name)) {
        fprintf(stderr, "Error: '%s' is not a valid SOBF file\n", file_name);
        return EXIT_FAILURE;
    }

    file_data prog;
    printf("\nlog1\n");

    extract_file_data(file_name, &prog);
    printf("\nlog1\n");

    // printf("%d %d\n", prog.c, prog.v);

    VM vm;

    init_vm(&vm, prog.values_table, prog.v, prog.codes_table, prog.c);

    run_vm(&vm);

    // 3. Affichage de l'état final si demandé
    if (argc >= 3 && strcmp(argv[2], "--print-end-machine") == 0) {
        print_machine_state(vm);
    }

    printf("Program finished.\n");

    // free allocated spaces
    free_file_data(&prog);
    // free(prog.codes_table);
    // free(prog.values_table);

    return 0;
}