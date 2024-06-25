// test.c
#include <stdio.h>
#include <vector>

extern double __enzyme_autodiff(void*, double);

class Function {
public:
    virtual double operator()(double x) = 0;

    virtual ~Function() {}
};

Function* func;

double square(double x) {

    return x * (*func)(x);
}

double dsquare(double x) {
    // This returns the derivative of square or 2 * x
//    return 3 * x * x;
    return __enzyme_autodiff((void*) square, x);
}

class Square : public Function {
public:
    double operator()(double x) {
        return x * x;
    }
};

int main() {

    func = new Square();

    for(double i=1; i<5; i++) {

        double check = (square(i + 1e-6) - square(i - 1e-6)) / (2 * 1e-6);

        printf("square(%f)=%f, dsquare(%f)=%f, check(%f)=%f\n", i, square(i), i, dsquare(i), i, check);

    }

    delete func;
}
