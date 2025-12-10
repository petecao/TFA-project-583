#include <iostream>

void performOp(int x, int y, int (*op)(int, int)) {
    std::cout << op(x, y) << "\n";   // indirect callback call
}

int add(int a, int b) { return a + b; }

int main() {
    performOp(3, 5, add);
}
