#pragma once

const double k = 1.4;
const double MaxLambda = sqrt((k + 1) / (k - 1));
const double Epsilon = 1e-3;
const int MaxIter = 50000;

double qfunc(double lambda);
double dqfunc(double lambda);
double lambdafunc(double q, int side);
