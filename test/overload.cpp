#include <iostream>

int foo(int){ return 1; }
double foo(double){ return 2.0; }

int main(){
    // choose overload explicitly by cast
    int (*fi)(int) = (int(*)(int)) &foo;
    double (*fd)(double) = (double(*)(double)) &foo;
    std::cout << fi(0) << " " << fd(0.0) << "\n";
}
