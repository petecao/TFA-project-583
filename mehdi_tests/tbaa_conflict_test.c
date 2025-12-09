// mehdi_tests/tbaa_fp_test.c

typedef void (*fp_t)(void);

int gi;
double gd;

// Two simple callbacks
void cb_int(void) {
    gi = 1;
}

void cb_double(void) {
    gd = 2.0;
}

// Two different “slots” that will (hopefully) get different TBAA types
struct IntSlot {
    int pad;      // int-typed data
    fp_t fp;
};

struct DoubleSlot {
    double pad;   // double-typed data
    fp_t fp;
};

struct IntSlot islot = { 0, cb_int };
struct DoubleSlot dslot = { 0.0, cb_double };

void drive(int cond) {
    fp_t fp;

    if (cond) {
        // access via int-typed struct
        islot.pad = 10;      // should carry int-ish TBAA
        fp = islot.fp;       // load function pointer from islot
    } else {
        // access via double-typed struct
        dslot.pad = 20.0;    // should carry double-ish TBAA
        fp = dslot.fp;       // load function pointer from dslot
    }

    fp();  // icall
}
