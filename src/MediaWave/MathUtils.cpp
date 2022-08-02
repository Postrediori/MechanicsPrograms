#include "pch.h"
#include "MathUtils.h"

int simq2(float a[2][2], float b[], float x[]) {
    float delta = a[0][0] * a[1][1] - a[1][0] * a[0][1];
    if (delta == 0.0) return 1;
    x[0] = (b[0] * a[1][1] - b[1] * a[0][1]) / delta;
    x[1] = (b[1] * a[0][0] - b[0] * a[1][0]) / delta;
    return 0;
}

bool betweenf(float a, float x, float b) {
    return ((a < x) && (x < b));
}