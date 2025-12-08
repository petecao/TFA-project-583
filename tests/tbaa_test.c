void foo() {}
void bar() {}

void test(int x) {
    void (*fp)();

    if (x > 0)
        fp = foo;
    else
        fp = bar;

    fp();   // <-- indirect call
}
