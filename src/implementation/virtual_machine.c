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
    vm -> global_size = v;
    init_stack(&vm->stack, 16);
    // Dans l'initialisation de la VM
	vm->atoms = malloc(256 * sizeof(long int));
	for (int i = 0; i < 256; i++) {
	    vm->atoms[i] = (long int)malloc(0); 
	}
}

/*
@requires le numéro de la primitive n, le nombre d'arguments p, et le tableau des arguments args
@ensures exécute la primitive correspondante et retourne le résultat
*/
long int call_primitive(int n, int p, long int args[]) {
	(void)p; // Supprime l'avertissement unused parameter
    switch (n) {
        
        case 15: { // Allocation de tableau : arg[0] = taille n, arg[1] = valeur de remplissage
            int size = (int)(args[0] >> 1); // Décodage de n
            long int fill_value = args[1];
            
            long int* array = malloc(size * sizeof(long int));
            if (array == NULL) {
                perror("malloc primitive 152");
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < size; i++) {
                array[i] = fill_value;
            }
            // On retourne le pointeur converti en long int
            return (long int)array;
        }

        case 288: { // fflush : arg[0] est un FILE *
            FILE* f = (FILE*)args[0];
            fflush(f);
            return 1; // Retourne l'unité (false/unit dans la VM)
        }

		case 293: { 
            FILE* f = (FILE*)args[0];
            int c = fgetc(f);
            if (c == EOF) return -1; // Correction warning shift-negative
            return (long int)((c << 1) | 1);
        }

        case 302: { // Flux d'entrée (stdin)
            if (args[0] == 1) { // L'énoncé dit "Si le premier paramètre est 1"
                return (long int)stdin;
            }
            return 1; // Valeur par défaut
        }

        case 304: { // Flux de sortie (stdout/stderr)
            if (args[0] == 3) {
                return (long int)stdout;
            } else if (args[0] == 5) {
                return (long int)stderr;
            }
            return 1;
        }

        case 310: { // fprintf(f, "%c", n) : arg[0] = FILE *, arg[1] = code ASCII
            FILE* f = (FILE*)args[0];
            int ascii_code = (int)(args[1] >> 1); // Décodage de n
            fprintf(f, "%c", ascii_code);
            return 1; // Retourne l'unité
        }

        default:
            fprintf(stderr, "Primitive %d non implémentée\n", n);
            return 1;
    }
}


/*
@requires a virtual_machine structure vm
@assigns vm->acc, vm->stack, vm->index
@ensures executes the virtual_machine
*/
void run_vm(VM* vm) {
    while (vm->codes[vm->index] != 143) { // 143 = STOP
        int code = vm->codes[vm->index];

        switch (code) {
            //////////////////// Basic instructions /////////////////
            case 0: case 1: case 2: case 3:
            case 4: case 5: case 6: case 7: // ACC0 à ACC7
                vm->acc = stack_seek(vm->stack, code);
                break;

            case 8: { // ACC n
                int n = vm->codes[vm->index + 1];
                vm->acc = stack_seek(vm->stack, n);
                vm->index += 2;
                continue;
            }
            case 9: // PUSH
                stack_push(&vm->stack, vm->acc);
                break;

            case 10: case 11: case 12: case 13:
            case 14: case 15: case 16: case 17: // PUSHACC0 à PUSHACC7
                stack_push(&vm->stack, stack_seek(vm->stack, code - 10));
                break;

            case 18: { // PUSHACC n
                int n = vm->codes[vm->index + 1];
                stack_push(&vm->stack, stack_seek(vm->stack, n));
                vm->index += 2;
                continue;
            }

            case 19: { // POP n
                int n = vm->codes[vm->index + 1];
                for (int i = 0; i < n; i++)
                    stack_pop(&vm->stack);
                vm->index += 2;
                continue;
            }

            case 20: { // ASSIGN n
                int n = vm->codes[vm->index + 1];
                vm->stack.data[vm->stack.size - 1 - n] = vm->acc;
                vm->acc = 1; // unité
                vm->index += 2;
                continue;
            }

            //////////////////// Branching /////////////////
            case 84: { // BRANCH
                int target = vm->codes[vm->index + 1];
                vm->index = target;
                continue;
            }

            case 85: { // BRANCHIF
                int target = vm->codes[vm->index + 1];
                if (vm->acc != 1) vm->index = target;
                else vm->index += 2;
                continue;
            }

            case 86: { // BRANCHIFNOT
                int target = vm->codes[vm->index + 1];
                if (vm->acc == 1) vm->index = target;
                else vm->index += 2;
                continue;
            }

            case 87: { // SWITCH
                int n_cases = vm->codes[vm->index + 1];
                int val_acc = (int)(vm->acc >> 1); // décodage entier
                if (val_acc >= 0 && val_acc < n_cases) {
                    vm->index = vm->codes[vm->index + 2 + val_acc];
                } else {
                    vm->index += n_cases + 2;
                }
                continue;
            }
			case 88: // BOOLNOT
			    if (vm->acc == 3) {
			        vm->acc = 1;
			    } else {
			        vm->acc = 3;
			    }
			    break;

			case 121: { // EQ
			    long int val_stack = stack_pop(&vm->stack);
			    
			    if (val_stack == vm->acc) {
			        vm->acc = 3; // true
			    } else {
			        vm->acc = 1; // false
			    }
			    break;
			}

			case 122: { // NEQ
			    long int val_stack = stack_pop(&vm->stack);
			    
			    if (val_stack != vm->acc) {
			        vm->acc = 3; // true
			    } else {
			        vm->acc = 1; // false
			    }
			    break;
			}

            case 90: { // CALL n
                int n = vm->codes[vm->index + 1];
                stack_push(&vm->stack, vm->index + 2); // sauvegarde PC
                vm->index = n;
                continue;
            }

            case 91: { // RETURN
                vm->index = stack_pop(&vm->stack);
                continue;
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
			    // On récupère l'entier n à la case suivante
			    int n = vm->codes[vm->index + 1];
			    // On met son encodage (2n + 1) dans l'accumulateur
			    vm->acc = (long int)(2 * n + 1);
			    // On avance de 2 (l'opcode + le paramètre n)
			    vm->index += 2;
			    continue; // Pour ne pas faire l'incrément automatique de la fin du while
			}

			case 104: // PUSHCONST0
			    stack_push(&vm->stack, vm->acc);
			    vm->acc = 1; // Encodage de 0
			    break;

			case 105: // PUSHCONST1
			    stack_push(&vm->stack, vm->acc);
			    vm->acc = 3; // Encodage de 1
			    break;

			case 106: // PUSHCONST2
			    stack_push(&vm->stack, vm->acc);
			    vm->acc = 5; // Encodage de 2
			    break;

			case 107: // PUSHCONST3
			    stack_push(&vm->stack, vm->acc);
			    vm->acc = 7; // Encodage de 3
			    break;
		    case 108: { // PUSHCONSTINT n
			    // 1. On empile l'accumulateur actuel
			    stack_push(&vm->stack, vm->acc);
			    // 2. On récupère n et on met son encodage dans l'acc
			    int n = vm->codes[vm->index + 1];
			    vm->acc = (long int)(2 * n + 1);
			    // 3. On avance l'indice de 2
			    vm->index += 2;
			    continue;
			}

			case 109: // NEGINT
			    // n_enc = 2n + 1. On veut 2(-n) + 1.
			    // Formule : 2 - (2n + 1) = 2 - 2n - 1 = -2n + 1 = 2(-n) + 1.
			    vm->acc = 2 - vm->acc;
			    break;

			case 110: { // ADDINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    // (2n+1) + (2m+1) = 2n + 2m + 2. 
			    // On soustrait 1 pour avoir 2(n+m) + 1.
			    vm->acc = n_enc + m_enc - 1;
			    break;
			}

			case 111: { // SUBINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    // Attention à l'ordre : acc contient n, la pile contient m. On veut n - m.
			    // (2n+1) - (2m+1) = 2n - 2m.
			    // On ajoute 1 pour avoir 2(n-m) + 1.
			    vm->acc = n_enc - m_enc + 1;
			    break;
			}

			case 112: { // MULINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    // Pour la multiplication, il est plus sûr de décoder au moins un des deux.
			    // n = (n_enc >> 1), m = (m_enc >> 1)
			    // Résultat = 2 * (n * m) + 1
			    long int n = n_enc >> 1;
			    long int m = m_enc >> 1;
			    vm->acc = (long int)((n * m) << 1) | 1;
			    break;
			}
			case 113: { // DIVINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    
			    // Décodage
			    long int n = n_enc >> 1;
			    long int m = m_enc >> 1;

			    if (m == 0) {
			        fprintf(stderr, "Fatal error: exception Division_by_zero\n");
			        exit(2);
			    }
			    // Division puis ré-encodage
			    vm->acc = ((n / m) << 1) | 1;
			    break;
			}

			case 114: { // MODINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    
			    long int n = n_enc >> 1;
			    long int m = m_enc >> 1;

			    if (m == 0) {
			        fprintf(stderr, "Fatal error: exception Division_by_zero\n");
			        exit(2);
			    }
			    vm->acc = ((n % m) << 1) | 1;
			    break;
			}

			case 115: { // ANDINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    // (2n+1) & (2m+1) donne bien 2(n&m)+1 car 1&1 = 1
			    vm->acc = n_enc & m_enc;
			    break;
			}

			case 116: { // ORINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    // (2n+1) | (2m+1) donne bien 2(n|m)+1 car 1|1 = 1
			    vm->acc = n_enc | m_enc;
			    break;
			}

			case 117: { // XORINT
			    long int m_enc = stack_pop(&vm->stack);
			    long int n_enc = vm->acc;
			    // (2n+1) ^ (2m+1) donne 2(n^m) + 0 car 1^1 = 0. 
			    // Il faut donc rajouter le bit de marquage (+1).
			    vm->acc = (n_enc ^ m_enc) | 1;
			    break;
			}
			/* --- Décalages de bits --- */
			case 118: { // LSLINT (n << m)
			    long int m = stack_pop(&vm->stack) >> 1;
			    long int n = vm->acc >> 1;
			    vm->acc = ((n << m) << 1) | 1;
			    break;
			}
			case 119: { // LSRINT (n >> m logique)
			    long int m = stack_pop(&vm->stack) >> 1;
			    unsigned long int n = (unsigned long int)(vm->acc >> 1); // Cast non signé
			    vm->acc = ((n >> m) << 1) | 1;
			    break;
			}
			case 120: { // ASRINT (n >> m arithmétique)
			    long int m = stack_pop(&vm->stack) >> 1;
			    long int n = vm->acc >> 1; // Le shift sur signed long est arithmétique en C
			    vm->acc = ((n >> m) << 1) | 1;
			    break;
			}

			/* --- Comparaisons (n < m, n <= m, etc.) --- */
			case 123: // LTINT
			    vm->acc = ((vm->acc >> 1) < (stack_pop(&vm->stack) >> 1)) ? 3 : 1;
			    break;
			case 124: // LEINT
			    vm->acc = ((vm->acc >> 1) <= (stack_pop(&vm->stack) >> 1)) ? 3 : 1;
			    break;
			case 125: // GTINT
			    vm->acc = ((vm->acc >> 1) > (stack_pop(&vm->stack) >> 1)) ? 3 : 1;
			    break;
			case 126: // GEINT
			    vm->acc = ((vm->acc >> 1) >= (stack_pop(&vm->stack) >> 1)) ? 3 : 1;
			    break;

			/* --- Comparaisons Non Signées --- */
			case 137: { // ULTINT
			    unsigned long int m = (unsigned long int)(stack_pop(&vm->stack) >> 1);
			    unsigned long int n = (unsigned long int)(vm->acc >> 1);
			    vm->acc = (n < m) ? 3 : 1;
			    break;
			}
			case 138: { // UGEINT
			    unsigned long int m = (unsigned long int)(stack_pop(&vm->stack) >> 1);
			    unsigned long int n = (unsigned long int)(vm->acc >> 1);
			    vm->acc = (n >= m) ? 3 : 1;
			    break;
			}

			/* --- Divers --- */
			case 127: { // OFFSETINT (n + m immédiat)
			    int m = vm->codes[vm->index + 1];
			    long int n_enc = vm->acc;
			    // n_enc est 2n+1. On veut 2(n+m)+1.
			    // 2(n+m)+1 = (2n+1) + 2m
			    vm->acc = n_enc + (2 * m);
			    vm->index += 2;
			    continue;
			}
			case 129: // ISINT
			    // Si le bit de poids faible est 1, c'est un entier (Impair)
			    vm->acc = (vm->acc & 1) ? 3 : 1;
			    break;
			/* --- Branchements Conditionnels avec Valeurs Immédiates --- */

			case 131: { // BEQ (n == m ?)
			    int m = vm->codes[vm->index + 1];
			    int c = vm->codes[vm->index + 2];
			    if ((vm->acc >> 1) == m) vm->index += c + 2;
			    else vm->index += 3;
			    continue;
			}

			case 132: { // BNEQ (n != m ?)
			    int m = vm->codes[vm->index + 1];
			    int c = vm->codes[vm->index + 2];
			    if ((vm->acc >> 1) != m) vm->index += c + 2;
			    else vm->index += 3;
			    continue;
			}

			case 133: { // BLTINT (m < n ?)
			    int m = vm->codes[vm->index + 1];
			    int c = vm->codes[vm->index + 2];
			    if (m < (vm->acc >> 1)) vm->index += c + 2;
			    else vm->index += 3;
			    continue;
			}

			case 134: { // BLEINT (m <= n ?)
			    int m = vm->codes[vm->index + 1];
			    int c = vm->codes[vm->index + 2];
			    if (m <= (vm->acc >> 1)) vm->index += c + 2;
			    else vm->index += 3;
			    continue;
			}

			case 135: { // BGTINT (m > n ?)
			    int m = vm->codes[vm->index + 1];
			    int c = vm->codes[vm->index + 2];
			    if (m > (vm->acc >> 1)) vm->index += c + 2;
			    else vm->index += 3;
			    continue;
			}

			case 136: { // BGEINT (m >= n ?)
			    int m = vm->codes[vm->index + 1];
			    int c = vm->codes[vm->index + 2];
			    if (m >= (vm->acc >> 1)) vm->index += c + 2;
			    else vm->index += 3;
			    continue;
			}

			case 139: { // BULTINT (m < n non signé)
			    unsigned int m = (unsigned int)vm->codes[vm->index + 1];
			    unsigned int c = (unsigned int)vm->codes[vm->index + 2];
			    unsigned long int n = (unsigned long int)(vm->acc >> 1);
			    if (m < n) vm->index += (int)c + 2;
			    else vm->index += 3;
			    continue;
			}

			case 140: { // BUGEINT (m >= n non signé)
			    unsigned int m = (unsigned int)vm->codes[vm->index + 1];
			    unsigned int c = (unsigned int)vm->codes[vm->index + 2];
			    unsigned long int n = (unsigned long int)(vm->acc >> 1);
			    if (m >= n) vm->index += (int)c + 2;
			    else vm->index += 3;
			    continue;
			}

            //////////////////// Primitives /////////////////
            /* --- Appels de fonctions primitives (C_CALL) --- */

			case 93: { // C_CALL1 n
			    int n = vm->codes[vm->index + 1];
			    // On appelle avec l'accumulateur comme seul argument
			    vm->acc = call_primitive(n, 1, (long int[]){vm->acc});
			    vm->index += 2;
			    continue;
			}

			case 94: { // C_CALL2 n
			    int n = vm->codes[vm->index + 1];
			    long int v2 = stack_pop(&vm->stack);
			    // Arguments : acc (v1) et v2
			    vm->acc = call_primitive(n, 2, (long int[]){vm->acc, v2});
			    vm->index += 2;
			    continue;
			}

			case 95: { // C_CALL3 n
			    int n = vm->codes[vm->index + 1];
			    long int v2 = stack_pop(&vm->stack);
			    long int v3 = stack_pop(&vm->stack);
			    vm->acc = call_primitive(n, 3, (long int[]){vm->acc, v2, v3});
			    vm->index += 2;
			    continue;
			}

			case 96: { // C_CALL4 n
			    int n = vm->codes[vm->index + 1];
			    long int v2 = stack_pop(&vm->stack);
			    long int v3 = stack_pop(&vm->stack);
			    long int v4 = stack_pop(&vm->stack);
			    vm->acc = call_primitive(n, 4, (long int[]){vm->acc, v2, v3, v4});
			    vm->index += 2;
			    continue;
			}

			case 97: { // C_CALL5 n
			    int n = vm->codes[vm->index + 1];
			    long int v2 = stack_pop(&vm->stack);
			    long int v3 = stack_pop(&vm->stack);
			    long int v4 = stack_pop(&vm->stack);
			    long int v5 = stack_pop(&vm->stack);
			    vm->acc = call_primitive(n, 5, (long int[]){vm->acc, v2, v3, v4, v5});
			    vm->index += 2;
			    continue;
			}

			case 98: { // C_CALLN p n
			    int p = vm->codes[vm->index + 1]; // Nombre total d'arguments
			    int n = vm->codes[vm->index + 2]; // Numéro de la primitive
			    
			    // On crée un tableau de taille p pour stocker les arguments
			    long int *args = malloc(p * sizeof(long int));
			    if (args == NULL) { perror("malloc"); exit(EXIT_FAILURE); }
			    
			    args[0] = vm->acc; // Le premier argument est toujours l'acc
			    for (int i = 1; i < p; i++) {
			        args[i] = stack_pop(&vm->stack); // Les autres sont dépilés
			    }
			    
			    vm->acc = call_primitive(n, p, args);
			    
			    free(args); // On libère le tableau temporaire
			    vm->index += 3;
			    continue;
			}


            //////////////////// Memory /////////////////
            /* --- Gestion des Variables Globales --- */

			case 53: { // GETGLOBAL n
			    int n = vm->codes[vm->index + 1];
			    // On charge la valeur globale n dans l'accumulateur
			    vm->acc = vm->globals[n];
			    vm->index += 2;
			    continue;
			}

			case 54: { // PUSHGETGLOBAL n
			    int n = vm->codes[vm->index + 1];
			    // 1. On empile l'accumulateur actuel
			    stack_push(&vm->stack, vm->acc);
			    // 2. On charge la valeur globale n dans l'accumulateur
			    vm->acc = vm->globals[n];
			    vm->index += 2;
			    continue;
			}

			case 57: { // SETGLOBAL n
			    int n = vm->codes[vm->index + 1];
			    // On met la valeur de l'accumulateur dans le tableau global n
			    vm->globals[n] = vm->acc;
			    // On met 1 (l'unité) dans l'accumulateur
			    vm->acc = 1;
			    vm->index += 2;
			    continue;
			}

			/* --- Création de Blocs (Tas / Heap) --- */

			case 62: { // MAKEBLOCK n
			    int n = vm->codes[vm->index + 1];
			    // On alloue un bloc de n valeurs
			    long int* block = malloc(n * sizeof(long int));
			    if (block == NULL) { perror("malloc MAKEBLOCK"); exit(EXIT_FAILURE); }

			    block[0] = vm->acc; // L'accumulateur va à l'indice 0
			    for (int i = 1; i < n; i++) {
			        block[i] = stack_pop(&vm->stack); // Les n-1 suivants sont dépilés
			    }
			    
			    vm->acc = (long int)block; // L'acc pointe maintenant vers le bloc
			    vm->index += 3; // On saute l'opcode, n, et la case ignorée
			    continue;
			}

			case 63: { // MAKEBLOCK1
			    long int* block = malloc(1 * sizeof(long int));
			    if (block == NULL) { perror("malloc MAKEBLOCK1"); exit(EXIT_FAILURE); }
			    
			    block[0] = vm->acc;
			    
			    vm->acc = (long int)block;
			    vm->index += 2; // On saute l'opcode et la case ignorée
			    continue;
			}

			case 64: { // MAKEBLOCK2
			    long int* block = malloc(2 * sizeof(long int));
			    if (block == NULL) { perror("malloc MAKEBLOCK2"); exit(EXIT_FAILURE); }
			    
			    block[0] = vm->acc;
			    block[1] = stack_pop(&vm->stack);
			    
			    vm->acc = (long int)block;
			    vm->index += 2;
			    continue;
			}

			case 65: { // MAKEBLOCK3
			    long int* block = malloc(3 * sizeof(long int));
			    if (block == NULL) { perror("malloc MAKEBLOCK3"); exit(EXIT_FAILURE); }
			    
			    block[0] = vm->acc;
			    block[1] = stack_pop(&vm->stack);
			    block[2] = stack_pop(&vm->stack);
			    
			    vm->acc = (long int)block;
			    vm->index += 2;
			    continue;
			}

			/* --- Accès aux champs (GETFIELD / SETFIELD) --- */
			case 67: vm->acc = ((long int*)vm->acc)[0]; break; // GETFIELD0
			case 68: vm->acc = ((long int*)vm->acc)[1]; break; // GETFIELD1
			case 69: vm->acc = ((long int*)vm->acc)[2]; break; // GETFIELD2
			case 70: vm->acc = ((long int*)vm->acc)[3]; break; // GETFIELD3

			case 71: { // GETFIELD n
			    int n = vm->codes[vm->index + 1];
			    vm->acc = ((long int*)vm->acc)[n];
			    vm->index += 2;
			    continue;
			}

			case 73: ((long int*)vm->acc)[0] = stack_pop(&vm->stack); vm->acc = 1; break; // SETFIELD0
			case 74: ((long int*)vm->acc)[1] = stack_pop(&vm->stack); vm->acc = 1; break; // SETFIELD1
			case 75: ((long int*)vm->acc)[2] = stack_pop(&vm->stack); vm->acc = 1; break; // SETFIELD2
			case 76: ((long int*)vm->acc)[3] = stack_pop(&vm->stack); vm->acc = 1; break; // SETFIELD3

			case 77: { // SETFIELD n
			    int n = vm->codes[vm->index + 1];
			    ((long int*)vm->acc)[n] = stack_pop(&vm->stack);
			    vm->acc = 1;
			    vm->index += 2;
			    continue;
			}

			/* --- Manipulation de Vecteurs (indices dynamiques) --- */
			case 80: { // GETVECTITEM (n est sur la pile)
			    int n = (int)(stack_pop(&vm->stack) >> 1); // Décodage de l'indice
			    vm->acc = ((long int*)vm->acc)[n];
			    break;
			}

			case 81: { // SETVECTITEM
			    int n = (int)(stack_pop(&vm->stack) >> 1); // Décodage de l'indice
			    long int v = stack_pop(&vm->stack);
			    ((long int*)vm->acc)[n] = v;
			    vm->acc = 1;
			    break;
			}

			/* --- Champs Globaux --- */
			case 55: { // GETGLOBALFIELD n p
			    int n = vm->codes[vm->index + 1];
			    int p = vm->codes[vm->index + 2];
			    long int* block = (long int*)vm->globals[n];
			    vm->acc = block[p];
			    vm->index += 3;
			    continue;
			}

			case 56: { // PUSHGETGLOBALFIELD n p
			    stack_push(&vm->stack, vm->acc);
			    int n = vm->codes[vm->index + 1];
			    int p = vm->codes[vm->index + 2];
			    long int* block = (long int*)vm->globals[n];
			    vm->acc = block[p];
			    vm->index += 3;
			    continue;
			}

			/* --- Références et Incrémentation --- */
			case 128: { // OFFSETREF n
			    int n = vm->codes[vm->index + 1];
			    long int* block = (long int*)vm->acc;
			    long int m_enc = block[0]; // Récupère l'entier encodé à l'indice 0
			    // m_enc + 2*n préserve l'encodage 2(m+n)+1
			    block[0] = m_enc + (2 * n);
			    vm->acc = 1;
			    vm->index += 2;
			    continue;
			}


            //////////////////// Atoms /////////////////
			case 58: // ATOM0
			    // On charge l'atome d'indice 0 dans l'accumulateur
			    vm->acc = vm->atoms[0];
			    break;

			case 59: { // ATOM
			    int n = vm->codes[vm->index + 1];
			    // On charge l'atome d'indice n (entre 0 et 255)
			    vm->acc = vm->atoms[n];
			    vm->index += 2;
			    continue;
			}

			case 60: // PUSHATOM0
			    // On empile l'accumulateur actuel, puis on charge l'atome 0
			    stack_push(&vm->stack, vm->acc);
			    vm->acc = vm->atoms[0];
			    break;

			case 61: // PUSHATOM n
			    int n = vm->codes[vm->index + 1];
			    // On empile l'accumulateur actuel, puis on charge l'atome n
			    stack_push(&vm->stack, vm->acc);
			    vm->acc = vm->atoms[n];
			    vm->index += 2;
			    continue;

            //////////////////// Stop the vm /////////////////
			case 143:
				exit(0);

            //////////////////// DEFAULT /////////////////
            default:
                fprintf(stderr, "Unknown opcode %d at index %d\n", code, vm->index);
                exit(EXIT_FAILURE);
        }

        vm->index += 1;
    } // end while if STOP instruction
}