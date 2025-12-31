#include <stdio.h>
#include "../header/stack.h"

int main(void) {
    Stack s;

    init_stack(&s, 5);

    stack_push(&s, 1);
    stack_push(&s, 2);
    stack_push(&s, 3);
    stack_push(&s, 4);
    print_stack(s);

    printf("size = %d\n", s.size);
    long int n = stack_pop(&s);
    printf("removed %ld\n", n);
    print_stack(s);
    printf("size = %d\n", s.size);

    int i = 0;
    long int d = stack_seek(s, i);
    printf("data[%d] = %ld\n", i, d);
    d = stack_seek(s, i + 1);
    printf("data[%d] = %ld\n", i + 1, d);
    d = stack_seek(s, i + 2);
    printf("data[%d] = %ld\n", i + 2, d);

    free_stack(&s);
    
    return 0;
}
