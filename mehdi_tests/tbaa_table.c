#include <stdint.h>

typedef void (*fp_t)(void);
volatile int sink;

#define N 16
#define MK_CB(N) void cb##N(void){ sink += (N); }

MK_CB(0) MK_CB(1) MK_CB(2) MK_CB(3)
MK_CB(4) MK_CB(5) MK_CB(6) MK_CB(7)
MK_CB(8) MK_CB(9) MK_CB(10) MK_CB(11)
MK_CB(12) MK_CB(13) MK_CB(14) MK_CB(15)

struct Slot0 { int a; fp_t fp; };
struct Slot1 { double a; fp_t fp; };
struct Slot2 { char a[8]; fp_t fp; };
struct Slot3 { long a; fp_t fp; };

struct Slot0 s0[N];
struct Slot1 s1[N];
struct Slot2 s2[N];
struct Slot3 s3[N];

static fp_t getfp(int i) {
    // Assign different functions into different base types
    s0[i].fp = cb0  + i; // (works because cbN are distinct symbols, but pointer arithmetic is UB)
    // Avoid UB: do a switch:
    switch (i & 15) {
        case 0: s0[i].fp = cb0; break;  case 1: s0[i].fp = cb1; break;
        case 2: s0[i].fp = cb2; break;  case 3: s0[i].fp = cb3; break;
        case 4: s1[i].fp = cb4; break;  case 5: s1[i].fp = cb5; break;
        case 6: s1[i].fp = cb6; break;  case 7: s1[i].fp = cb7; break;
        case 8: s2[i].fp = cb8; break;  case 9: s2[i].fp = cb9; break;
        case 10: s2[i].fp = cb10; break; case 11: s2[i].fp = cb11; break;
        case 12: s3[i].fp = cb12; break; case 13: s3[i].fp = cb13; break;
        case 14: s3[i].fp = cb14; break; case 15: s3[i].fp = cb15; break;
    }

    fp_t fp;
    // Join across different slots/types
    if (i & 1) fp = s0[i].fp; else fp = s1[i].fp;
    if (i & 2) fp = s2[i].fp;
    if (i & 4) fp = s3[i].fp;
    return fp;
}

int main(int argc, char **argv) {
    fp_t fp = getfp(((uintptr_t)argv >> 3) ^ argc);
    fp();
    return 0;
}
