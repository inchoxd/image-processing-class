#include <iostream>
#include <cmath>

class complex {         // ex. class for complex numbers
    private:
        double real;    // real part
        double imag;    // imaginary part
    public:
        complex() : real(-1.0), imag(-1.0){}                        // Constructor
        complex(double a, double b) : real(a), imag(b){}
        double abs() { return sqrt(real * real + imag * imag); }
};

int main(int argc, char** argv) {
    // instance of c++ class
    complex *a;
    complex b(5, 7);
    a = &b;
    std::cout << "absolute value = " << a->abs() << std::endl;

    return 0;
}
