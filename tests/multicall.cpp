#include <iostream>

int add(int a, int b) { return a + b; }
int mul(int a, int b) { return a * b; }

int main() {
    int (*ops[])(int, int) = { add, mul };
    
    for (auto f : ops) {
        std::cout << f(3, 4) << "\n";   // Indirect calls
    }
}