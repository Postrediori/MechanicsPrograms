#include "pch.h"
#include "FluidModel.h"

double qfunc(double lambda) {
    double q1, q2;
    q1 = pow((1 - lambda * lambda * (k - 1) / (k + 1)), 1 / (k - 1));
    q2 = pow((2 / (k + 1)), (-1 / (k - 1)));
    return lambda * q1 * q2;
}

double dqfunc(double lambda) {
    double q1, q2;
    q1 = pow((1 - lambda * lambda * (k - 1) / (k + 1)), 1 / (k - 1) - 1);
    q2 = pow((2 / (k + 1)), (-1 / (k - 1)));
    return (1 - lambda * lambda) * q1 * q2;
}

double lambdafunc(double q, int side) {
    int i;
    double xn, oldxn;

    if (side == 1) xn = 0.5;
    else xn = 1.5;

    i = 0;
    while (1) {
        oldxn = xn;
        xn = oldxn - (qfunc(oldxn) - q) / dqfunc(oldxn);
        if ((fabs(xn - oldxn) < Epsilon) || (i > MaxIter)) break;
        i++;
    }

    return xn;
}
