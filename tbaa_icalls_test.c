#include <stdlib.h>
#include <stdio.h>

// Two distinct structs. 
// In C/C++ TBAA, 'StructA' and 'StructB' are distinct branches of the type tree.
typedef struct { 
    void (*f)(void); 
} StructA;

typedef struct { 
    void (*g)(void); 
} StructB;

// Target functions
void funcA() { printf("Called A\n"); }
void funcB() { printf("Called B\n"); }

// The driver: Takes generic memory (opaque pointer)
void drive(void* mem) {
    StructA* a = (StructA*)mem;
    StructB* b = (StructB*)mem;

    // 1. STORE A: Clang attaches !tbaa "StructA" to this store
    // This sets up the alias node for 'a' with Tag A.
    a->f = funcA; 

    // 2. STORE B: Clang attaches !tbaa "StructB" to this store
    // This sets up the alias node for 'b' with Tag B.
    b->g = funcB;

    // 3. INDIRECT CALL via A
    // TFA ALONE: Sees 'mem' flowing to both 'a' and 'b'. It merges them.
    //            Result: Call targets = {funcA, funcB} (IMPRECISE)
    //
    // WITH TBAA: 'a' has Tag A. 'b' has Tag B. 
    //            The merge between 'a' and 'b' is BLOCKED.
    //            Result: Call targets = {funcA} (PRECISE)
    a->f(); 
}

int main() {
    // Allocate raw memory. This single allocation is the "source" 
    // that usually confuses TFA into merging everything.
    void* m = malloc(sizeof(StructA)); 
    drive(m);
    free(m);
    return 0;
}