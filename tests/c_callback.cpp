#include <iostream>
#include <functional>

using Cb = void(*)(int);

thread_local std::function<void(int)> TL_CB;

extern "C" void trampoline(int x){
    if(TL_CB) TL_CB(x);
}

void register_callback(Cb c){ /* imagine C API registers this */ (void)c; }

int main(){
    TL_CB = [](int x){ std::cout << "called " << x << "\n"; };
    register_callback(&trampoline); // pass C function pointer that calls a C++ std::function
    trampoline(7); // direct test
}
