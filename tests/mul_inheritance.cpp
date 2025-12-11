#include <iostream>

struct A { virtual void v(){ std::cout << "A\n"; } };
struct B { virtual void v(){ std::cout << "B\n"; } };

struct C : A, B {
    void v() override { std::cout << "C\n"; }
};

int main(){
    C c;
    // pointer-to-member to B::v (which must do an adjustment when invoked on C object)
    void (B::*pmf)() = &B::v;
    B* pb = &c;  // pointer to B subobject
    (pb->*pmf)(); // calls C::v via vtable + correct pointer adjustment
}