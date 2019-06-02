// Func.cpp

#include <math.h>
#include "Func.h"

int signum(double x) {
    if (x==0.0) return 0;
    else if (x<0.0) return -1;
    else return 1;
}

double func(double x, double y) {
    return (x*x + y - 11)*(x*x + y - 11) + (x + y*y - 7)*(x + y*y - 7);
}

/* double func(double x, double y) {
    y -= 2.0;
    return (y-x*x)*(y-x*x)/750+(1-x)*(1-x)/50*(y-1)*(y-1)/30;
} */
