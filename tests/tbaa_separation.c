#include <stdlib.h>

// Two distinct structs with different TBAA paths
typedef struct { void (*f)(void); } StructA;
typedef struct { void (*g)(void); } StructB;

void func_for_A() { /* Target A */ }
void func_for_B() { /* Target B */ }

// The optimizer will likely inline this, so we use noinline to keep the
// arguments distinct in the IR for the analysis to see.
__attribute__((noinline))
void distinct_processor(StructA *a, StructB *b) {
    // 1. Store to 'a'. Tag: StructA
    a->f = func_for_A;

    // 2. Store to 'b'. Tag: StructB
    // In opaque pointers, 'a' and 'b' look like generic 'ptr'.
    // Without TBAA, the analysis thinks 'b' aliases 'a', so this overwrite
    // might affect 'a'.
    b->g = func_for_B;

    // 3. Indirect Call via 'a'
    // Result WITHOUT TBAA: {func_for_A, func_for_B} (Spurious merge)
    // Result WITH TBAA:    {func_for_A}             (Precise!)
    a->f();
}

int main() {
    // Allocate generic memory
    void *mem = malloc(sizeof(StructA));
    
    // Pass the SAME pointer to both arguments.
    // This technically violates strict aliasing if both are accessed, 
    // which gives TBAA permission to say "They do not alias".
    distinct_processor((StructA*)mem, (StructB*)mem);
    
    free(mem);
    return 0;
}