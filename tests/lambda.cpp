#include <iostream>
#include <functional>

int main() {
    std::function f = [](int x) { return x + 10; };
    std::cout << f(7) << "\n"; // indirect call
}