#include <iostream>
#include <functional>

// Simulated C API that accepts void(*)(void*) and a context pointer.
using Ccb = void(*)(void*);
void register_cb(Ccb, void*){/* pretend */}

// trampoline forwards to std::function stored in heap
extern "C" void trampoline(void* ctx){
    auto* fn = static_cast<std::function<void()>*>(ctx);
    (*fn)();
}

int main(){
    auto* fn = new std::function<void()>( []{ std::cout << "captured called\n"; } );
    register_cb(&trampoline, fn);
    trampoline(fn); // test invocation
    delete fn;
}
