#include <iostream>
#include <atomic>
#include <thread>

int f1(){ return 1; }
int f2(){ return 2; }

std::atomic<int(*)()> current{&f1};

void worker(){
    for(int i=0;i<5;++i){
        int(*fn)() = current.load();
        std::cout << fn() << " ";
    }
}

int main(){
    std::thread t(worker);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    current.store(&f2);
    t.join();
}
