#pragma once

class SimplexEngine: public SearchEngine {
public:
    SimplexEngine();

    void draw(CoordinateFunc xFunc, CoordinateFunc yFunc) override;

    void search_start() override;
    void search_step() override;

private:
    void update_min();

private:
    Simplex simplex_;
    std::vector<Simplex> history_;
};
