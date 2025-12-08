#include <iostream>

void hello1() {
    std::cout << "Hello1\n";
}

void hello2() {
    std::cout << "Hello2\n";
}


int main() {
    void (*func_ptr)();
    func_ptr = &hello1;

    func_ptr(); 
    
    func_ptr = &hello2;
    func_ptr();

    return 0;
}