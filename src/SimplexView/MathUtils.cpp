#include "pch.h"
#include "MathUtils.h"

int signum(double x) {
    if (x > 0.0) {
        return 1;
    }
    else if (x < 0.0) {
        return -1;
    }
    return 0;
}

bool almost_equal(double a, double b) {
    return abs(a - b) < Epsilon;
}
