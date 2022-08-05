#include "pch.h"
#include "SurfaceFunction.h"

double surface_func_linear(double x, double delta, double epsilon) {
    return epsilon * (x / delta - 0.5);
}

double surface_func_sine(double x, double delta, double epsilon) {
    return epsilon * sin(2 * M_PI * x / delta);
}

double surface_func_cosine(double x, double delta, double epsilon) {
    return epsilon * cos(2 * M_PI * x / delta);
}

double surface_func_polygonal(double x, double delta, double epsilon) {
    return (x < delta * 0.25) ? (epsilon) : ((x > delta * 0.75) ? (-epsilon) : (epsilon - 4.0 * epsilon * (x - delta * 0.25) / (delta)));
}
