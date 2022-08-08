#pragma once

class RelaxationEngine: public SearchEngine {
public:
    RelaxationEngine();

    void draw(CoordinateFunc xFunc, CoordinateFunc yFunc) override;

    void search_start() override;
    void search_step() override;

private:
    double x_{ 0.0 }, y_{ 0.0 };
    double xold_{ 0.0 }, yold_{ 0.0 };
    double dx_{ 0.0 }, dy_{ 0.0 };
    double ddx_{ 0.0 }, ddy_{ 0.0 };
    double xlen_{ 0.0 }, ylen_{ 0.0 };
    double sx_{ 0.0 }, sy_{ 0.0 };
    int currentVar_{ 0 };
};
