#pragma once

class ScanEngine: public SearchEngine {
public:
    ScanEngine();

    void draw(CoordinateFunc xfunc, CoordinateFunc yfunc) override;

    void search_start() override;
    void search_step() override;

private:
    double dx{ 0.0 }, dy{ 0.0 };
};
