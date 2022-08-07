#pragma once

class GaussSEngine: public SearchEngine {
public:
    GaussSEngine();

    void draw(CoordinateFunc xfunc, CoordinateFunc yfunc) override;

    void search_start() override;
    void search_step() override;

private:
    int currentVar{ 0 };

    double ddx{ 0.0 }, ddy{ 0.0 };
    double x{ 0.0 }, y{ 0.0 };
    double xold{ 0.0 }, yold{ 0.0 };
};
