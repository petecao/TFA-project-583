// POSIX-only example; link with -ldl on Linux
#include <iostream>
#include <dlfcn.h>

int main(){
    void* lib = dlopen(nullptr, RTLD_NOW); // RTLD_DEFAULT could be used
    if(!lib){ std::cerr << "dlopen failed\n"; return 1; }
    // get address of puts from libc as example:
    using puts_t = int(*)(const char*);
    puts_t p = (puts_t)dlsym(lib, "puts");
    if(p) p("Hello from dlsym!");
    return 0;
}
