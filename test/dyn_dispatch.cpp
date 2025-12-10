#include <iostream>
#include <functional>
#include <unordered_map>

int main() {
    std::unordered_map<int, std::function<void()>> table;

    table[0] = [] { std::cout << "Zero\n"; };
    table[1] = [] { std::cout << "One\n"; };

    int opcode = 1;
    table[opcode]();   // indirect call based on runtime opcode
}
