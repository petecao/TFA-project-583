#include <iostream>

int f(int x){ return x + 1; }

int main(){
    int (*fp)(int) = &f;
    int (**ppf)(int) = &fp;
    std::cout << (*ppf)(41) << "\n"; // calls f
}
