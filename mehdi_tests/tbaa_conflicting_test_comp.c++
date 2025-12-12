// tbaa_icall_stress.c
#include <stdint.h>


typedef void (*fp_void_t)(void);
typedef void (*fp_int_t)(int);

// Global side-effect targets
int    g_int    = 0;
double g_double = 0.0;
float  g_float  = 0.0f;
long   g_long   = 0;

// Callback implementations
void cb_reset(void)       { g_int = 0; g_double = 0.0; }
void cb_set_int(void)     { g_int = 42; }
void cb_set_double(void)  { g_double = 3.14159; }
void cb_set_float(void)   { g_float = 1.23f; }
void cb_set_long(void)    { g_long = 0xBADC0FFEE; }
void cb_with_arg(int x)   { g_int = x; }



struct IntSlot {
    int     pad;      // Type: int
    fp_void_t fp;
};

struct DoubleSlot {
    double  pad;      // Type: double
    fp_void_t fp;
};

struct IntSlot    s_int    = { 1, cb_set_int };
struct DoubleSlot s_double = { 2.0, cb_set_double };

void test_strict_separation(int cond) {
    fp_void_t func;
    
    s_int.fp = cb_set_int;
    s_double.fp = cb_set_double;

    // Both paths load a function pointer, but the "base" object differs in type.
    if (cond) {
        // [IntSlot TBAA Node]
        s_int.pad = 100;
        func = s_int.fp; 
    } else {
        // [DoubleSlot TBAA Node]
        s_double.pad = 200.0;
        func = s_double.fp;
    }

    // CHECK: Does analysis realize loads from s_int and s_double are distinct?
    func(); 
}

struct Inner {
    float x;
    fp_void_t fp;
};

struct Outer {
    long y;
    struct Inner in;
};

struct Outer g_outer = { 0, { 0.0f, cb_set_float } };

void test_nested_structs(void) {
    // Access deep field
    g_outer.in.x = 5.5f; 
    
    g_outer.in.fp = cb_set_float;
    // Load function pointer from nested path
    fp_void_t f = g_outer.in.fp;
    
    // CHECK: Should not alias with SCENARIO 1 loads
    f();
}


union Punner {
    struct IntSlot    as_int;
    struct DoubleSlot as_double;
    fp_void_t         raw_fp;
};

union Punner g_pun = { .as_int = { 1, cb_set_long } };

void test_union_punning(int mode) {
    fp_void_t f;

    // We are attacking the type system here. All these loads access the 
    // exact same bytes in memory.
    g_pun.as_int.fp = cb_set_long;
    g_pun.as_double.fp = cb_set_float;
    g_pun.raw_fp = nullptr;
    if (mode == 0) {
        g_pun.as_int.pad = 5;
        f = g_pun.as_int.fp;
    } else if (mode == 1) {
        g_pun.as_double.pad = 9.9;
        f = g_pun.as_double.fp;
    } else {
        // Direct access, bypassing struct wrappers
        f = g_pun.raw_fp;
    }

    // CHECK: Analysis MUST treat these sources as aliasing/overlapping.
    // If it treats them as distinct based solely on type, it is unsafe here.
    f();
}


struct ArrayElem {
    char c;
    fp_int_t handler;
};

struct ArrayElem g_arr[5] = {
    { 'a', cb_with_arg }, { 'b', cb_with_arg }, { 'c', cb_with_arg },
    { 'd', cb_with_arg }, { 'e', cb_with_arg }
};

void test_array_indexing(int idx) {
    if (idx < 0 || idx >= 5) return;

    g_arr[idx].c = 'z';

    // Load via GEP with non-zero index
    fp_int_t f = g_arr[idx].handler;

    f(idx);
}



struct WrapperA { int id; fp_void_t fp; };
struct WrapperB { float val; fp_void_t fp; };

struct WrapperA wa = { 1, cb_reset };
struct WrapperB wb = { 1.0f, cb_set_float };

// Helper receives raw void*, erasing the type info at the interface
void helper_runner(void* ctx, int is_type_a) {
    fp_void_t f;
    if (is_type_a) {
        struct WrapperA* p = (struct WrapperA*)ctx;
        f = p->fp; // Type info restored to WrapperA
    } else {
        struct WrapperB* p = (struct WrapperB*)ctx;
        f = p->fp; // Type info restored to WrapperB
    }

    f();
}

void test_void_polymorphism(void) {
    helper_runner(&wa, 1);
    helper_runner(&wb, 0);
}


int main(int argc, char** argv) {
    test_strict_separation(argc % 2);
    test_nested_structs();
    test_union_punning(argc % 3);
    test_array_indexing(2);
    test_void_polymorphism();
    return 0;
}