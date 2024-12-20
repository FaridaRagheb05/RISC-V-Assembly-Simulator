#include <iostream>
using namespace std;



int main(){

    uint32_t x1, x2, x3, x4, x5, x6; // to represent unsigned integers (registers) that are 32 bits long
    x1 = 50 << 12; // Load upper immediate (50) into x1
    x2 = x1 >> 2; // SRLI x2, x1, 2  (x2 = x1 >> 2 (logical shift of x1 twice and placing value in x2))
    x3 = x1 ^ x2; // XOR x3, x1, x2  (x3 = x1 ^ x2)
    x4 = x3 | x1; // OR x4, x3, x1 (x4 = x3 | x1)
    x5 = x4 >> x2; // SRA x5, x4, x2 (x5 = x4 >> x2 (arithmetic shift of x4 by value in x2))
    x6 = x5 << 1; // SLLI x6, x5, 1 (x6 = x5 << 1 / logical shift of x5 once and placing value in x6)
}