#include <iostream>

struct Base {
    virtual void run() { std::cout << "Base\n"; }
};

struct Derived : Base {
    void run() override { std::cout << "Derived\n"; }
};

int main() {
    Base* obj = new Derived;  
    obj->run(); // indirect virtual call (vtable)
    delete obj;
}