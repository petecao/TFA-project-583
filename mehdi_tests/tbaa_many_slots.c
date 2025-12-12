#include <stdint.h>

typedef void (*fp_t)(void);

volatile int sink;

#define MK_CB(N) \
void cb##N(void){ sink += (N); }

MK_CB(0)  MK_CB(1)  MK_CB(2)  MK_CB(3)  MK_CB(4)
MK_CB(5)  MK_CB(6)  MK_CB(7)  MK_CB(8)  MK_CB(9)
MK_CB(10) MK_CB(11) MK_CB(12) MK_CB(13) MK_CB(14)
MK_CB(15) MK_CB(16) MK_CB(17) MK_CB(18) MK_CB(19)

struct SlotI { int pad; fp_t fp; };
struct SlotD { double pad; fp_t fp; };
struct SlotC { char pad[24]; fp_t fp; };
struct SlotL { long pad; fp_t fp; };

struct SlotI si = {0, cb0};
struct SlotD sd = {0.0, cb1};
struct SlotC sc = {{0}, cb2};
struct SlotL sl = {0, cb3};

static fp_t pick(int k) {
    fp_t fp;
    // Stores into different base types / slots -> different TBAA tags
    si.fp = cb4;
    sd.fp = cb5;
    sc.fp = cb6;
    sl.fp = cb7;

    // Create joins that would encourage merges
    if (k & 1) fp = si.fp;
    else      fp = sd.fp;

    if (k & 2) fp = fp;
    else      fp = sc.fp;

    if (k & 4) fp = sl.fp;
    return fp;
}

int main(int argc, char **argv) {
    fp_t fp = pick((argc << 1) ^ (uintptr_t)argv);
    fp(); // icall
    return 0;
}
