#include <stdio.h>
#include <stdlib.h>
#include "../header/stack.h"
#include "../header/file_reader.h"

int main(int argc, char* argv[]) {
    // chack number of variables, must equal to 2
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        exit(1);
    }



    return 0;
}