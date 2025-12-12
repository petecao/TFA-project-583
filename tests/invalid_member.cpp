#include <iostream>

struct A { virtual void f(){ std::cout << "A\n"; } };
struct B { virtual void f(){ std::cout << "B\n"; } };

int main(){
    A a;
    // reinterpret_cast B* from A* then call B::f via vtable — UB
    B* pb = reinterpret_cast<B*>(&a);
    pb->f(); // undefined behavior
}
