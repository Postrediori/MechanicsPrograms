#pragma once

class ScanEngine: public SearchEngine {
public:
    ScanEngine();

    void draw(CoordinateFunc xFunc, CoordinateFunc yFunc) override;

    void search_start() override;
    void search_step() override;

private:
    double dx_{ 0.0 }, dy_{ 0.0 };
};
