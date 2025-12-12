#include <stdio.h>
#include <stdlib.h>

struct A { long x; void (*cb)(void); };
struct B { char y; void (*cb)(void); };

void func_A() { printf("A\n"); }
void func_B() { printf("B\n"); }

// A wrapper that takes a void pointer. 
// To a flow analyzer, 'ptr' could be ANYTHING.
void run_callback(void *ptr, int is_struct_a) {
    if (is_struct_a) {
        // Cast to A: The analyzer sees flow from (void*) -> (A*)
        struct A *a = (struct A*)ptr;
        a->cb(); 
    } else {
        // Cast to B: The analyzer sees flow from (void*) -> (B*)
        struct B *b = (struct B*)ptr;
        b->cb();
    }
}

int main() {
    struct A a = {0, func_A};
    struct B b = {'z', func_B};

    // Both pass through the SAME void* argument.
    // TFA will likely merge these nodes: {func_A, func_B} -> void* -> {call A, call B}
    run_callback(&a, 1);
    run_callback(&b, 0);

    return 0;
}