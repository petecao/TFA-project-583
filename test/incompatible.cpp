#include <iostream>

int f(int a){ return a + 10; }

int main(){
    // reinterpret_cast to incompatible signature
    double (*fp)(double) = reinterpret_cast<double(*)(double)>( &f );
    // UB when called, but some platforms will just reinterpret bits.
    std::cout << fp(1.0) << "\n";
}