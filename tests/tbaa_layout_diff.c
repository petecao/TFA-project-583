#include <stdlib.h>

// StructA: Contains Function Pointer at Offset 0
typedef struct {
    void (*f)(void);
} StructA;

// StructB: Contains Dummy Data, then Function Pointer at Offset 8
// This FORCES the TBAA tag to be different because the access offset is different.
typedef struct {
    long padding; 
    void (*g)(void);
} StructB;

void func_A() { /* Target A */ }
void func_B() { /* Target B */ }

// Disable inlining to ensure the analysis sees the function boundary
__attribute__((noinline))
void distinct_processor(StructA *a, StructB *b) {
    // 1. Store to 'a' (Offset 0). Tag: "StructA", "FunctionPtr", Offset 0
    a->f = func_A;

    // 2. Store to 'b' (Offset 8). Tag: "StructB", "FunctionPtr", Offset 8
    // These tags are now GUARANTEED to be different.
    b->g = func_B;

    // 3. Indirect Call via 'a'
    // Without TBAA: The analyzer sees 'a' and 'b' pointing to the same 'mem', merges them.
    //               Result -> {func_A, func_B}
    // With TBAA:    The analyzer sees "StructA" cannot alias "StructB". Merge Blocked.
    //               Result -> {func_A}
    a->f();
}

int main() {
    // Allocate enough space for the larger struct
    void *mem = malloc(sizeof(StructB));
    
    // Pass the same buffer to both
    distinct_processor((StructA*)mem, (StructB*)mem);
    
    free(mem);
    return 0;
}