#include "pch.h"
#include "MathUtils.h"
#include "FuncUtils.h"

double func(double x, double y) {
    constexpr double a0 = -11.0;
    constexpr double b0 = -7.0;
    
    double a = x * x + y + a0;
    double b = x + y * y + b0;

    return a * a + b * b;
}

/* double func(double x, double y) {
    y -= 2.0;
    return (y-x*x)*(y-x*x)/750+(1-x)*(1-x)/50*(y-1)*(y-1)/30;
} */

double dfdx(double x, double y) {
    return (func(x + Epsilon, y) - func(x, y)) / Epsilon;
}

double dfdy(double x, double y) {
    return (func(x, y + Epsilon) - func(x, y)) / Epsilon;
}
