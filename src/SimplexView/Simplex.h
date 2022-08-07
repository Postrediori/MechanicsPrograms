#pragma once

constexpr int SimplexOrder = 3;

struct Simplex {
    Simplex() = default;

    void set(float x1, float y1, float x2, float y2, float x3, float y3);
    void setTriangle(float size, float x, float y);

    void flip(int node);
    void reduce(int node);

    int getMaxNode() const;
    int getMinNode() const;
    float getSize() const;
    vec4 getMinPoint() const;

    static bool compare_points(vec4 a, vec4 b) {
        return a.z < b.z;
    }

    std::array<vec4, SimplexOrder> points;
    int max_node{ -1 };
};
