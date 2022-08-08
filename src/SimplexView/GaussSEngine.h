#pragma once

class GaussSEngine: public SearchEngine {
public:
    GaussSEngine();

    void draw(CoordinateFunc xFunc, CoordinateFunc yFunc) override;

    void search_start() override;
    void search_step() override;

private:
    double x_{ 0.0 }, y_{ 0.0 };
    double xold_{ 0.0 }, yold_{ 0.0 };
    double ddx_{ 0.0 }, ddy_{ 0.0 };
    int currentVar_{ 0 };
};
