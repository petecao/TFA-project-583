#include <stdint.h>

typedef void (*fp_t)(void);
volatile int sink;

void fa(void){ sink += 1; }
void fb(void){ sink += 2; }
void fc(void){ sink += 3; }
void fd(void){ sink += 4; }

struct A { int pad; fp_t cb; };
struct B { double pad; fp_t cb; };

struct A ga;
struct B gb;

// The void* funnel
static fp_t load_cb(void *p, int which) {
    if (which) {
        struct A *a = (struct A*)p;
        return a->cb;  // load w/ TBAA for A.cb
    } else {
        struct B *b = (struct B*)p;
        return b->cb;  // load w/ TBAA for B.cb
    }
}

int main(int argc, char **argv) {
    ga.cb = fa;
    gb.cb = fb;

    // extra stores to create more candidates
    if (argc & 1) ga.cb = fc;
    if (argc & 2) gb.cb = fd;

    fp_t fp = load_cb((argc & 1) ? (void*)&ga : (void*)&gb, argc & 1);
    fp();
    return 0;
}