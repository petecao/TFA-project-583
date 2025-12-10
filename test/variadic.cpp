#include <iostream>
#include <cstdarg>

int sum(int n, ...){
    va_list ap;
    va_start(ap, n);
    int s=0;
    for(int i=0;i<n;++i) s += va_arg(ap,int);
    va_end(ap);
    return s;
}

int main(){
    int (*vfp)(int,...) = &sum;
    std::cout << vfp(3,1,2,3) << "\n"; // indirect varargs call
}
