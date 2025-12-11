#include <iostream>
#include <cstring>

int main(){
    unsigned char buf[64];
    memset(buf, 0x90, sizeof(buf)); // NOP-like bytes but not valid code
    auto fp = reinterpret_cast<void(*)()>(buf);
    fp(); // attempt to execute stack/data → segfault or W^X violation
}
