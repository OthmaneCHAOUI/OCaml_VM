#include <stdio.h>
#include <stdlib.h>
#include "../header/virtual_machine.h"

/*
@resuires virtual machine structure, globales table and its size, codes table and its size, a stack
@assigns vm->index, vm->acc, vm->codes, vm->codes_size, vm->globals, vm->globals_size, vm->stack
@ensures initializes the virtual machine
*/
void init_vm(VM* vm, long int* globals, int v, int* codes, int c) {
    vm -> index = 0;
    vm -> acc = 1;
    vm -> codes = codes;
    vm -> globals = globals;
    vm -> codes_size = c;
    vm -> globals_size = v;
    init_stack(&vm->stack, 16);
    // stack_push(&vm->stack, 1); // initialize the stack, because some instructions access data from the stack from the get go
	vm->atoms = malloc(256 * sizeof(long int));
	for (int i = 0; i < 256; i++) {
	    vm->atoms[i] = (long int)malloc(0);
	}
}

/*
@requires number of primitive, size of table, table
@assigns nothing
@ensures executes the called primitives depending on their number
*/
long int call_primitive(int prim, int argc, long int* argv) {
	(void)argc;
    switch (prim) {
        case 15: {
            int n = (int)((argv[0] - 1) / 2);

            long int* tab = malloc(n * sizeof(long int));
            if (!tab) { perror("malloc"); exit(EXIT_FAILURE); }

            for (int i = 0; i < n; i++) {
                tab[i] = argv[1];
            }

            return (long int)tab;
        }
        case 288: {
            FILE* f = (FILE*)argv[0];  
            fflush(f);
            return 1; 
        }
        case 293: {
            FILE* f = (FILE*)argv[0];
            int c = fgetc(f);
            return (long int)(2 * c + 1); 
        }
        case 302:
            if (argv[0] == 1)
                return (long int)stdin;
            return 1;
        case 304:
            if (argv[0] == 3)
                return (long int)stdout;
            if (argv[0] == 5)
                return (long int)stderr;
            return 1;
        case 310: {
            FILE* f = (FILE*)argv[0];
            int c = (int)((argv[1] - 1) / 2);
            fputc(c, f);
            return 1;
        }

        default:
            fprintf(stderr, "Unknown primitive %d\n", prim);
            exit(EXIT_FAILURE);
    }
}

/*
@requires a virtual_machine structure vm
@assigns vm->index, vm->acc, vm->codes, vm->codes_size, vm->globals, vm->globals_size, vm->stack
@ensures executes the virtual_machine
*/
void run_vm(VM* vm) {
    while (vm->codes[vm->index] != 143 && vm->index < vm->codes_size) { // stop the loop if encounter code 143(STOP) or vm->index is at the last element of the codes table
        int code = vm->codes[vm->index];

        // printf("PC: %2d | Opcode: %3d | ACC: %15ld | Stack size: %d\n", vm->index, code, vm->acc, vm->stack.size); // to see changes in data and detect problems

        switch (code) {
            //////////////////// Basic instructions /////////////////
            case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: // ACC0 to ACC7
                vm->acc = stack_seek(vm->stack, code);
                break;
            case 8: // ACC
                int n = vm->codes[vm->index + 1];
                vm->acc = stack_seek(vm->stack, n);
                vm->index += 2;
                continue;
            case 9: // PUSH
                stack_push(&vm->stack, vm->acc);
                break;
            case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: // PUSHACC0 to PUSHACC7
            	long int old_acc = vm->acc;
            	// should push then seek because the stack is initially empty
                stack_push(&vm->stack, old_acc);
                vm->acc = stack_seek(vm->stack, code - 10);
			    break;
            case 18: { // PUSHACC
                int n = vm->codes[vm->index + 1];
                stack_push(&vm->stack, vm->acc);
                vm->acc = stack_seek(vm->stack, n);
                vm->index += 2;
                continue;
            }
            case 19: { // POP
                int n = vm->codes[vm->index + 1];
                for (int i = 0; i < n; i++)
                    stack_pop(&vm->stack);
                vm->index += 2;
                continue;
            }
            case 20: { // ASSIGN
                int n = vm->codes[vm->index + 1];
                vm->stack.data[vm->stack.size - 1 - n] = vm->acc;
                vm->acc = 1; // unité
                vm->index += 2;
                continue;
            }

            //////////////////// Branching /////////////////
            case 84: { // BRANCH
                int n = vm->codes[vm->index + 1];
                vm->index += n + 1;
                continue;
            }

            case 85: { // BRANCHIF
                int n = vm->codes[vm->index + 1];
                if (vm->acc != 1) vm->index += n + 1;
                else vm->index += 2;
                continue;
            }

            case 86: { // BRANCHIFNOT
                int n = vm->codes[vm->index + 1];
                if (vm->acc == 1) vm->index += n+1;
                else vm->index += 2;
                continue;
            }

            case 87: { // SWITCH
                int n = vm->codes[vm->index + 1];
                int val_acc = (int)((vm->acc - 1) / 2); // acc = 2*i + 1 => i = (acc - 1) / 2

                if (val_acc >= 0 && val_acc < n) {
                    vm->index += vm->codes[vm->index + 2 + val_acc] + 2;
                }
                else {
                    vm->index += n + 2;
                }
                continue;
            }
			case 88: // BOOLNOT
			    if (vm->acc == 3) {
			        vm->acc = 1;
			    }
			    else {
			        vm->acc = 3;
			    }
			    break;

			case 121: { // EQ
			    long int val_stack = stack_pop(&vm->stack);
			    
			    if (val_stack == vm->acc) {
			        vm->acc = 3; // true
			    }
			    else {
			        vm->acc = 1; // false
			    }
			    break;
			}

			case 122: { // NEQ
			    long int val_stack = stack_pop(&vm->stack);
			    
			    if (val_stack != vm->acc) {
			        vm->acc = 3; // true
			    }
			    else {
			        vm->acc = 1; // false
			    }
			    break;
			}

            //////////////////// Integers /////////////////
            case 99: // CONST0
			    vm->acc = 1;
			    break;
			case 100: // CONST1
			    vm->acc = 3;
			    break;

			case 101: // CONST2
			    vm->acc = 5;
			    break;

			case 102: // CONST3
			    vm->acc = 7;
			    break;
			case 103: { // CONSTINT n
			    int n = vm->codes[vm->index + 1];
			    vm->acc = (long int)(2 * n + 1);
			    vm->index += 2;
			    continue;
			}

			case 104: // PUSHCONST0
			    stack_push(&vm->stack, vm->acc);
			    vm->acc = 1;
			    break;

			case 105: // PUSHCONST1
			    stack_push(&vm->stack, vm->acc);
			    vm->acc = 3;
			    break;

			case 106: // PUSHCONST2
			    stack_push(&vm->stack, vm->acc);
			    vm->acc = 5;
			    break;

			case 107: // PUSHCONST3
			    stack_push(&vm->stack, vm->acc);
			    vm->acc = 7;
			    break;
		    case 108: { // PUSHCONSTINT
			    int n = vm->codes[vm->index + 1];
			    stack_push(&vm->stack, vm->acc);
			    vm->acc = (long int)(2 * n + 1);
			    vm->index += 2;
			    continue;
			}

			case 109: // NEGINT
			    // formula : 2 - (2n + 1) = 2 - 2n - 1 = -2n + 1 = 2(-n) + 1
			    vm->acc = 2 - vm->acc;
			    break;

			case 110: { // ADDINT
			    long int m = stack_pop(&vm->stack);
			    long int n = vm->acc;
			    // formula : (2n + 1) + (2m + 1) - 1 = 2n + 2m + 2 - 1 = 2(n + m) + 1
			    vm->acc = n + m - 1;
			    break;
			}

			case 111: { // SUBINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    // formula : (2n + 1) - (2m + 1) + 1 = 2n - 2m + 1 = 2(n - m) + 1
			    vm->acc = n_enc - m_enc + 1;
			    break;
			}

			case 112: { // MULINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    // formula : 2mn + 1 = ((n_enc - 1) / 2) * (m_enc - 1) + 1 knowing that n_enc = 2n + 1 and m_enc = 2m + 1
			    vm->acc = ((n_enc - 1) / 2) * (m_enc - 1) + 1;
			    break;
			}

			case 113: { // DIVINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    long int n = (n_enc - 1) / 2;
			    long int m = (m_enc - 1) / 2;

			    if (m == 0) {
			        fprintf(stderr, "Fatal error: exception Division_by_zero\n");
			        exit(2);
			    }
			    vm->acc = 2 * (n / m) + 1;
			    break;
			}

			case 114: { // MODINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    long int n = (n_enc - 1) / 2;
			    long int m = (m_enc - 1) / 2;

			    if (m == 0) {
			        fprintf(stderr, "Fatal error: exception Division_by_zero\n");
			        exit(2);
			    }
			    vm->acc = (n % m) * 2 + 1;
			    break;
			}

			case 115: { // ANDINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    long int m = (m_enc - 1) / 2;
				long int n = (n_enc - 1) / 2;
			    vm->acc = 2 * (m & n) + 1;
			    break;
			}

			case 116: { // ORINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    long int m = (m_enc - 1) / 2;
				long int n = (n_enc - 1) / 2;
			    vm->acc = 2 * (m | n) + 1;
			    break;
			}

			case 117: { // XORINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    long int m = (m_enc - 1) / 2;
				long int n = (n_enc - 1) / 2;
			    vm->acc = 2 * (m ^ n) + 1;
			    break;
			}

			case 118: { // LSLINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    long int n = (n_enc - 1) / 2;
			    long int m = (m_enc - 1) / 2;

			    long int res = n;
			    for (int i = 0; i < m; i++) res *= 2;

			    vm->acc = res * 2 + 1;
			    break;
			}

			case 119: { // LSRINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    unsigned long int n = (unsigned long int)((n_enc - 1) / 2);
			    long int m = (m_enc - 1) / 2;

			    unsigned long int res = n;
			    for (int i = 0; i < m; i++) res /= 2;

			    vm->acc = (long int)res * 2 + 1;
			    break;
			}

			case 120: { // ASRINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    long int n = (n_enc - 1) / 2;
			    long int m = (m_enc - 1) / 2;

			    long int res = n;
			    for (int i = 0; i < m; i++) res /= 2;

			    vm->acc = res * 2 + 1;
			    break;
			}

			case 123: { // LTINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    long int n = (n_enc - 1) / 2;
			    long int m = (m_enc - 1) / 2;
			    if (n < m) vm->acc = 3;
			    else vm->acc = 1;
			    break;
			}

			case 124: { // LEINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    long int n = (n_enc - 1) / 2;
			    long int m = (m_enc - 1) / 2;
			    if (n <= m) vm->acc = 3;
			    else vm->acc = 1;
			    break;
			}

			case 125: { // GTINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    long int n = (n_enc - 1) / 2;
			    long int m = (m_enc - 1) / 2;
			    if (n > m) vm->acc = 3;
			    else vm->acc = 1;
			    break;
			}

			case 126: { // GEINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    long int n = (n_enc - 1) / 2;
			    long int m = (m_enc - 1) / 2;
			    if (n >= m) vm->acc = 3;
			    else vm->acc = 1;
			    break;
			}

			case 137: { // ULTINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    unsigned long int n = (unsigned long int)((n_enc - 1) / 2);
			    unsigned long int m = (unsigned long int)((m_enc - 1) / 2);
			    if (n < m) vm->acc = 3;
			    else vm->acc = 1;
			    break;
			}

			case 138: { // UGEINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    unsigned long int n = (unsigned long int)((n_enc - 1) / 2);
			    unsigned long int m = (unsigned long int)((m_enc - 1) / 2);
			    if (n >= m) vm->acc = 3;
			    else vm->acc = 1;
			    break;
			}

			case 127: { // OFFSETINT
			    int m = vm->codes[vm->index + 1];
			    long int n_enc = vm->acc;
			    long int n = (n_enc - 1) / 2;
			    vm->acc = (n + m) * 2 + 1;
			    vm->index += 2;
			    continue;
			}

			case 129: { // ISINT
			    if (vm->acc % 2 != 0) {
			        vm->acc = 3; // true
			    } else {
			        vm->acc = 1; // false
			    }
			    break;
			}

			case 131: { // BEQ
			    int m = vm->codes[vm->index + 1];
			    int c = vm->codes[vm->index + 2];
			    long int n = (vm->acc - 1) / 2;

			    if (n == m) vm->index += c + 2;
			    else vm->index += 3;
			    continue;
			}

			case 132: { // BNEQ
			    int m = vm->codes[vm->index + 1];
			    int c = vm->codes[vm->index + 2];
			    long int n = (vm->acc - 1) / 2;

			    if (n != m) vm->index += c + 2;
			    else vm->index += 3;
			    continue;
			}

			case 133: { // BLTINT
			    int m = vm->codes[vm->index + 1];
			    int c = vm->codes[vm->index + 2];
			    long int n = (vm->acc - 1) / 2;

			    if (m < n) vm->index += c + 2;
			    else vm->index += 3;
			    continue;
			}

			case 134: { // BLEINT
			    int m = vm->codes[vm->index + 1];
			    int c = vm->codes[vm->index + 2];
			    long int n = (vm->acc - 1) / 2;

			    if (m <= n) vm->index += c + 2;
			    else vm->index += 3;
			    continue;
			}

			case 135: { // BGTINT
			    int m = vm->codes[vm->index + 1];
			    int c = vm->codes[vm->index + 2];
			    long int n = (vm->acc - 1) / 2;

			    if (m > n) vm->index += c + 2;
			    else vm->index += 3;
			    continue;
			}

			case 136: { // BGEINT
			    int m = vm->codes[vm->index + 1];
			    int c = vm->codes[vm->index + 2];
			    long int n = (vm->acc - 1) / 2;

			    if (m >= n) vm->index += c + 2;
			    else vm->index += 3;
			    continue;
			}

			case 139: { // BULTINT
			    unsigned int m = (unsigned int)vm->codes[vm->index + 1];
			    int c = vm->codes[vm->index + 2];
			    unsigned long int n = (unsigned long int)((vm->acc - 1) / 2);

			    if (m < n) vm->index += c + 2;
			    else vm->index += 3;
			    continue;
			}

			case 140: { // BUGEINT
			    unsigned int m = (unsigned int)vm->codes[vm->index + 1];
			    int c = vm->codes[vm->index + 2];
			    unsigned long int n = (unsigned long int)((vm->acc - 1) / 2);

			    if (m >= n) vm->index += c + 2;
			    else vm->index += 3;
			    continue;
			}

            //////////////////// Primitives /////////////////
			case 93: { // C_CALL1
			    int n = vm->codes[vm->index + 1];
			    vm->acc = call_primitive(n, 1, &vm->acc);
			    vm->index += 2;
			    continue;
			}

			case 94: { // C_CALL2
			    int n = vm->codes[vm->index + 1];
			    long int v2 = stack_pop(&vm->stack);
			    long int args[2] = { vm->acc, v2 };

			    vm->acc = call_primitive(n, 2, args);
			    vm->index += 2;
			    continue;
			}

			case 95: { // C_CALL3
			    int n = vm->codes[vm->index + 1];
			    long int v2 = stack_pop(&vm->stack);
			    long int v3 = stack_pop(&vm->stack);
			    long int args[3] = { vm->acc, v2, v3 };

			    vm->acc = call_primitive(n, 3, args);
			    vm->index += 2;
			    continue;
			}

			case 96: { // C_CALL4
			    int n = vm->codes[vm->index + 1];
			    long int v2 = stack_pop(&vm->stack);
			    long int v3 = stack_pop(&vm->stack);
			    long int v4 = stack_pop(&vm->stack);
			    long int args[4] = { vm->acc, v2, v3, v4 };

			    vm->acc = call_primitive(n, 4, args);
			    vm->index += 2;
			    continue;
			}

			case 97: { // C_CALL5
			    int n = vm->codes[vm->index + 1];
			    long int v2 = stack_pop(&vm->stack);
			    long int v3 = stack_pop(&vm->stack);
			    long int v4 = stack_pop(&vm->stack);
			    long int v5 = stack_pop(&vm->stack);
			    long int args[5] = { vm->acc, v2, v3, v4, v5 };

			    vm->acc = call_primitive(n, 5, args);
			    vm->index += 2;
			    continue;
			}

			case 98: { // C_CALLN
			    int p = vm->codes[vm->index + 1];
			    int n = vm->codes[vm->index + 2];

			    long int* args = malloc(p * sizeof(long int));
			    if (!args) { perror("malloc"); exit(EXIT_FAILURE); }

			    args[0] = vm->acc;
			    for (int i = 1; i < p; i++) {
			        args[i] = stack_pop(&vm->stack);
			    }

			    vm->acc = call_primitive(n, p, args);

			    free(args);
			    vm->index += 3;
			    continue;
			}

            //////////////////// Memory /////////////////
			case 53: {
			    int n = vm->codes[vm->index + 1];
			    vm->acc = vm->globals[n];
			    vm->index += 2;
			    continue;
			}

			case 54: {
			    int n = vm->codes[vm->index + 1];
			    stack_push(&vm->stack, vm->acc);
			    vm->acc = vm->globals[n];
			    vm->index += 2;
			    continue;
			}

			case 57: {
			    int n = vm->codes[vm->index + 1];
			    vm->globals[n] = vm->acc;
			    vm->acc = 1;
			    vm->index += 2;
			    continue;
			}

			case 62: {
			    int n = vm->codes[vm->index + 1];
			    long int *tab = malloc(n * sizeof(long int));
			    tab[0] = vm->acc;
			    for (int i = 1; i < n; i++) {
			        tab[i] = stack_pop(&vm->stack);
			    }
			    vm->acc = (long int)tab;
			    vm->index += 3;
			    continue;
			}

			case 63: {
			    long int *tab = malloc(1 * sizeof(long int));
			    tab[0] = vm->acc;
			    vm->acc = (long int)tab;
			    vm->index += 2;
			    continue;
			}

			case 64: {
			    long int *tab = malloc(2 * sizeof(long int));
			    tab[0] = vm->acc;
			    tab[1] = stack_pop(&vm->stack);
			    vm->acc = (long int)tab;
			    vm->index += 2;
			    continue;
			}

			case 65: {
			    long int *tab = malloc(3 * sizeof(long int));
			    tab[0] = vm->acc;
			    tab[1] = stack_pop(&vm->stack);
			    tab[2] = stack_pop(&vm->stack);
			    vm->acc = (long int)tab;
			    vm->index += 2;
			    continue;
			}

			case 67: {
			    long int *tab = (long int *)vm->acc;
			    vm->acc = tab[0];
			    break;
			}

			case 68: {
			    long int *tab = (long int *)vm->acc;
			    vm->acc = tab[1];
			    break;
			}

			case 69: {
			    long int *tab = (long int *)vm->acc;
			    vm->acc = tab[2];
			    break;
			}

			case 70: {
			    long int *tab = (long int *)vm->acc;
			    vm->acc = tab[3];
			    break;
			}
			case 71: {
			    int n = vm->codes[vm->index + 1];
			    long int *tab = (long int *)vm->acc;
			    vm->acc = tab[n];
			    vm->index += 2;
			    continue;
			}

			case 73: {
			    long int *tab = (long int *)vm->acc;
			    long int v = stack_pop(&vm->stack);
			    tab[0] = v;
			    vm->acc = 1;
			    break;
			}

			case 74: {
			    long int *tab = (long int *)vm->acc;
			    long int v = stack_pop(&vm->stack);
			    tab[1] = v;
			    vm->acc = 1;
			    break;
			}

			case 75: {
			    long int *tab = (long int *)vm->acc;
			    long int v = stack_pop(&vm->stack);
			    tab[2] = v;
			    vm->acc = 1;
			    break;
			}

			case 76: {
			    long int *tab = (long int *)vm->acc;
			    long int v = stack_pop(&vm->stack);
			    tab[3] = v;
			    vm->acc = 1;
			    break;
			}

			case 77: {
			    int n = vm->codes[vm->index + 1];
			    long int *tab = (long int *)vm->acc;
			    long int v = stack_pop(&vm->stack);
			    tab[n] = v;
			    vm->acc = 1;
			    vm->index += 2;
			    continue;
			}

			case 80: {
			    long int n_enc = stack_pop(&vm->stack);
			    long int n = (n_enc - 1) / 2;
			    long int *tab = (long int *)vm->acc;
			    vm->acc = tab[n];
			    break;
			}

			case 81: {
			    long int n_enc = stack_pop(&vm->stack);
			    long int n = (n_enc - 1) / 2;
			    long int v = stack_pop(&vm->stack);
			    long int *tab = (long int *)vm->acc;
			    tab[n] = v;
			    vm->acc = 1;
			    break;
			}

			case 55: {
			    int n = vm->codes[vm->index + 1];
			    int p = vm->codes[vm->index + 2];
			    long int *tab = (long int *)vm->globals[n];
			    vm->acc = tab[p];
			    vm->index += 3;
			    continue;
			}

			case 56: {
			    stack_push(&vm->stack, vm->acc);
			    int n = vm->codes[vm->index + 1];
			    int p = vm->codes[vm->index + 2];
			    long int *tab = (long int *)vm->globals[n];
			    vm->acc = tab[p];
			    vm->index += 3;
			    continue;
			}

			case 128: {
			    int n = vm->codes[vm->index + 1];
			    long int *tab = (long int *)vm->acc;
			    long int m_enc = tab[0];
			    long int m = (m_enc - 1) / 2;
			    tab[0] = (m + n) * 2 + 1;
			    vm->acc = 1;
			    vm->index += 2;
			    continue;
			}

            //////////////////// Atoms /////////////////
			case 58: {
			    vm->acc = vm->atoms[0];
			    break;
			}

			case 59: {
			    int n = vm->codes[vm->index + 1];
			    vm->acc = vm->atoms[n];
			    vm->index += 2;
			    continue;
			}

			case 60: {
			    stack_push(&vm->stack, vm->acc);
			    vm->acc = vm->atoms[0];
			    break;
			}

			case 61: {
			    stack_push(&vm->stack, vm->acc);
			    int n = vm->codes[vm->index + 1];
			    vm->acc = vm->atoms[n];
			    vm->index += 2;
			    continue;
			}
			case 92: // CHECK_SIGNALS
				// does nothing
			    break;

            //////////////////// Stop the vm /////////////////
			// case 143:
			// 	exit(0);

            //////////////////// DEFAULT /////////////////
            default:
                fprintf(stderr, "Unknown opcode %d at index %d\n", code, vm->index);
                exit(EXIT_FAILURE);
        }
        vm->index++;
    }
}

/*
@requires a valid and not empty VM vm
@assigns nothing
@ensures prints the state of the virtual machine (used with --print-end-machine flag)
*/
void print_machine_state(VM vm) {
    printf("Index: %d\n", vm.index);
    printf("Accumulator: %ld\n", vm.acc);
    printf("Stack:\n");
    print_stack(vm.stack);
    printf("Global:\n");
    for (int i = 0; i < vm.globals_size; i++) {
        printf("%d %ld\n", i, vm.globals[i]);
    }
}