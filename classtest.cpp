#include <iostream>

class complex {     // ex. class for complex numbers
    double real;    // real part
    double imag;    // imaginary part
};

int main(int argc, char** argv) {
    // c++クラスのインスタンス
    complex a;
    a.real = 1.5;
    a.imag = -1.0;

    return 0;
}

