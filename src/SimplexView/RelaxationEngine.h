#pragma once

class RelaxationEngine: public SearchEngine {
public:
    RelaxationEngine();

    void draw(CoordinateFunc xfunc, CoordinateFunc yfunc) override;

    void search_start() override;
    void search_step() override;

private:
    double x{ 0.0 }, y{ 0.0 }, ddx{ 0.0 }, ddy{ 0.0 }, dx{ 0.0 }, dy{ 0.0 };
    double xlen{ 0.0 }, ylen{ 0.0 }, sx{ 0.0 }, sy{ 0.0 };
    int currentVar{ 0 };
    double xold{ 0.0 }, yold{ 0.0 };
};
