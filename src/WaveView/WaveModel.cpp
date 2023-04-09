#include "pch.h"
#include "SurfaceFunction.h"
#include "WaveModel.h"

WaveModel::WaveModel() {
    xn = 30;
    zn = 15;
    maxn = 50;

    g = 9.81;
    delta = 1.0;
    epsilon = 0.1;
    h = 0.3;

    dtime = 0.01;

    surface_func = surface_func_linear;

    kn.resize(maxn);
    an.resize(maxn);
    sigman2.resize(maxn);
    sigman.resize(maxn);

    points.resize(xn * zn);

    reset();
}

double WaveModel::fourier_n(double d, int n) {
    constexpr int MaxIter = 1000;

    double minx = 0.0;
    double maxx = d;
    double dx = (maxx - minx) / double(MaxIter);
    double s = 0.0;
    for (int i = 0; i < MaxIter + 1; i++) {
        double xi = minx + dx * i;
        double f = surface_func(xi, delta, epsilon) * cos(kn[n - 1] * xi);
        int k = (i == 0 || i == MaxIter) ? 1 : 2 + (i % 2) * 2;
        s += f * static_cast<double>(k);
    }

    s = s * dx / 3.0;

    return 2.0 * s / d;
}

void WaveModel::calc_a() {
    for (int i = 1; i <= maxn; i++) {
        an[i - 1] = fourier_n(delta, i);
    }
}

void WaveModel::calc_coeffs() {
    for (int i = 1; i <= maxn; i++) {
        kn[i - 1] = M_PI * double(i) / delta;
        sigman2[i - 1] = g * kn[i - 1] * tanh(kn[i - 1] * h);
        sigman[i - 1] = sqrt(sigman2[i - 1]);
    }

    calc_a();
}

double WaveModel::Gxn(double z0, double t, int n) {
    return -kn[n - 1] * an[n - 1] * cosh(kn[n - 1] * (z0 + h)) *
        cos(sigman[n - 1] * t) / (sigman2[n - 1] * cosh(kn[n - 1] * h));
}

double WaveModel::Gzn(double z0, double t, int n) {
    return -kn[n - 1] * an[n - 1] * sinh(kn[n - 1] * (z0 + h)) *
        cos(sigman[n - 1] * t) / (sigman2[n - 1] * cosh(kn[n - 1] * h));
}

double WaveModel::Fx(double x0, double z0, double t) {
    double sum = 0.0;
    for (int i = 1; i <= maxn; i++) {
        sum += Gxn(z0, t, i) * sin(kn[i - 1] * x0);
    }
    return g * sum;
}

double WaveModel::Fz(double x0, double z0, double t) {
    double sum = 0.0;
    for (int i = 1; i <= maxn; i++) {
        sum += Gzn(z0, t, i) * cos(kn[i - 1] * x0);
    }
    return g * sum;
}

void WaveModel::reset() {
    time = 0.0;
    frame = 0;

    calc_coeffs();

    double dx = delta / double(xn - 1);
    double dz = h / double(zn - 1);

    for (int i = 0; i < xn; i++) {
        for (int j = 0; j < zn; j++) {
            Point p{ 0 };
            p.x0 = dx * double(i);
            p.z0 = -dz * double(j);
            p.x = p.x0 + Fx(p.x0, p.z0, time);
            p.z = p.z0 - Fz(p.x0, p.z0, time);
            points[i * zn + j] = p;
        }
    }
}

void WaveModel::step() {
    frame++;
    time = double(frame) * dtime;
    for (int i = 0; i < xn; i++) {
        for (int j = 0; j < zn; j++) {
            Point p{ points[i * zn + j] };
            p.x = p.x0 + Fx(p.x0, p.z0, time);
            p.z = p.z0 - Fz(p.x0, p.z0, time);
            points[i * zn + j] = p;
        }
    }
}
