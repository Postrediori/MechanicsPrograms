#pragma once

struct WaveModel {
    struct Point {
        double x, z;
        double x0, z0;
    };

    WaveModel();

    void reset();
    void step();

    double g;
    double delta, epsilon, h;

    double time, dtime;
    int frame;

    int xn, zn;
    int maxn;

    std::vector<double> kn, an, sigman2, sigman;
    std::vector<Point> points;

    SurfaceFunc surface_func;
    double fourier_n(double d, int n);
    void calc_a();
    void calc_coeffs();
    double Gxn(double z0, double t, int n);
    double Gzn(double z0, double t, int n);
    double Fx(double x0, double z0, double t);
    double Fz(double x0, double z0, double t);
};
