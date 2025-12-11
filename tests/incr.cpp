#include <iostream>

struct Math {
    int inc(int x) { return x + 1; }
};

int main() {
    Math obj;
    int (Math::*fp)(int) = &Math::inc;   // pointer to member function
    std::cout << (obj.*fp)(10) << "\n";  // indirect call
}