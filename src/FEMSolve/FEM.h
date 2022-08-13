#pragma once

constexpr int ModelDimensions = 2;

struct Node {
    int node;
    float x, y;
};

struct Fixture {
    int node;
    int axis;
};

struct Load {
    int node;
    float px, py;
};

struct Element {
    int elem;
    int nodes[ModelDimensions];
    float length, length2;
    float sina, cosa;
};

struct FinitModel {
    FinitModel() = default;
    ~FinitModel();

    void load_from_file(const std::string& fileName);
    void clear();

    void prepare_solve();
    void solve();

    int component_count{ 0 };

    std::vector<Node> nodes;
    std::vector<Fixture> fixes;
    std::vector<Load> loads;
    std::vector<Element> elems;

    Matrix a;
    Vector b;
    Vector u;
    Vector n;

    double E{ 1 }, F{ 1 };

    std::string title;
};
