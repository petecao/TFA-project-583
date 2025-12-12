#include <iostream>
#include <functional>

int square(int x) { return x * x; }

int main() {
    std::function f = square;  // type-erased indirect call
    std::cout << f(5) << "\n";           // indirect call
}