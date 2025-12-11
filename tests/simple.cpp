#include <iostream>

int add(int a, int b) { return a + b; }

int main() {
    int (*fp)(int, int) = &add;    // Point to function
    std::cout << fp(3, 4) << "\n"; // Indirect call
}