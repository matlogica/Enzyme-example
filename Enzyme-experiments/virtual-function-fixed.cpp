// test.c
#include <stdio.h>
#include <vector>

int enzyme_dup, enzyme_const, enzyme_out, enzyme_dupnoneed;

template <typename... T>
double __enzyme_autodiff(void *, T...);

template <typename T>
T __enzyme_virtualreverse(T);


class Function {
public:
    virtual double operator()(double x) = 0;

    virtual ~Function() {}
};


double square(double x, Function* func) {

    return x * (*func)(x);
}

double dsquare(double x, Function* func, Function* dfunc) {
    // This returns the derivative of square or 2 * x
    //    return 3 * x * x;
    return (double) __enzyme_autodiff((void*) square, x, enzyme_dup, func, dfunc);
}

class Square : public Function {
public:
    double operator()(double x) {
        return x * x;
    }
};

int main() {

    Function* func = new Square();
    Function* dfunc = new Square();
    *((void**)dfunc) = __enzyme_virtualreverse(*(void**)dfunc);

    for(double i=1; i<5; i++) {

        double check = (square(i + 1e-6, func) - square(i - 1e-6, func)) / (2 * 1e-6);

        printf("square(%f)=%f, dsquare(%f)=%f, check(%f)=%f\n", i, square(i, func), i, dsquare(i, func, dfunc), i, check);

    }
}