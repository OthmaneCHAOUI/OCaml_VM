#include <stdio.h>
#include "../header/stack.h"

int main(void) {
    Stack s;

    init_stack(&s, 5);

    stack_push(&s, 1);
    stack_push(&s, 2);
    stack_push(&s, 3);
    stack_push(&s, 3);
    print_stack(s);

    long int n = stack_pop(&s);
    printf("%ld\n", n);
    print_stack(s);


    free_stack(&s);
    
    return 0;
}
