#pragma once

class DescentEngine: public SearchEngine {
public:
    DescentEngine();

    void draw(CoordinateFunc xfunc, CoordinateFunc yfunc) override;

    void search_start() override;
    void search_step() override;

private:
    double x{ 0.0 }, y{ 0.0 };
    double ddx{ 0.0 }, ddy{ 0.0 };
    double xold{ 0.0 }, yold{ 0.0 };
};
