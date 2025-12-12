#include <stdio.h>

// Two different struct types, each with a callback at a *different* offset
struct SlotA {
    int padA;          // offset 0
    void (*cb)(void);  // offset 4 or 8
};

struct SlotB {
    double padB;       // offset 0 (8 bytes)
    void (*cb)(void);  // offset 8 or 16
};

void func_A(void) { printf("A\n"); }
void func_B(void) { printf("B\n"); }

// Global instances – they live in disjoint memory regions.
struct SlotA g_slotA;
struct SlotB g_slotB;

// Initialize callbacks in a "generic-looking" way.
void init(int which) {
    if (which) {
        g_slotA.cb = func_A;
    } else {
        g_slotB.cb = func_B;
    }
}

// Call site 1: logically should only ever call func_A.
void callA(void) {
    // The analysis sees: load from g_slotA.cb, then indirect call.
    void (*fp)(void) = g_slotA.cb;
    fp();
}

// Call site 2: logically should only ever call func_B.
void callB(void) {
    void (*fp)(void) = g_slotB.cb;
    fp();
}

int main(void) {
    // Drive both sides
    init(1);
    init(0);

    callA();
    callB();
    return 0;
}
