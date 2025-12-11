#include <iostream>
#include <vector>

template<int N>
int addN(int x){ return x + N; }

int main(){
    // array of different template instantiations
    int (*table[])(int) = { addN<0>, addN<1>, addN<2>, addN<3> };
    for(auto fn : table) std::cout << fn(10) << " ";
    std::cout << "\n";
}
