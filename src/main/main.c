#include <stdio.h>
#include "../header/stack.h"

int main(void) {
    Stack s;

    init_stack(&s, 2);

    stack_push(&s, 10);
    stack_push(&s, 20);
    stack_push(&s, 30);  // test resize

    print_stack(s);

    free_stack(&s);
    
    return 0;
}
