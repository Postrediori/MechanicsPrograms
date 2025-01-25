#include "pch.h"
#include "MathUtils.h"
#include "FuncUtils.h"
#include "Simplex.h"

void Simplex::set(float x1, float y1, float x2, float y2, float x3, float y3) {
    points[0] = HMM_Vec4{ x1, y1, static_cast<float>(func(x1, y1)), 0 };
    points[1] = HMM_Vec4{ x2, y2, static_cast<float>(func(x2, y2)), 0 };
    points[2] = HMM_Vec4{ x3, y3, static_cast<float>(func(x3, y3)), 0 };
    max_node = -1;
}

void Simplex::setTriangle(float size, float x, float y) {
    HMM_Vec2 p0 = HMM_Vec2{ x, y };
    for (int i=0; i<SimplexOrder; i++) {
        double angle = i * 2 * M_PI / SimplexOrder;
        HMM_Vec2 p = p0 + HMM_Vec2{ cosf(angle), sinf(angle) } *size;
        points[i] = HMM_Vec4{ p.X, p.Y, 0, 0 };
    }
    max_node = -1;
}

int Simplex::getMaxNode() const {
    auto iter = std::max_element(points.begin(), points.end(), Simplex::compare_points);
    return std::distance(points.begin(), iter);
}

int Simplex::getMinNode() const {
    auto iter = std::min_element(points.begin(), points.end(), Simplex::compare_points);
    return std::distance(points.begin(), iter);
}

float Simplex::getSize() const {
    float sum = 0.f;
    for (size_t i = 0; i<points.size(); i++) {
        size_t j = (i + 1) % points.size();
        HMM_Vec2 d = points[i].XY - points[j].XY;
        sum += HMM_LenV2(d);
    }
    sum /= static_cast<float>(points.size());
    return sum;
}

HMM_Vec4 Simplex::getMinPoint() const {
    return points.at(getMinNode());
}

void Simplex::flip(int node) {
    if (node == -1) {
        return;
    }
    HMM_Vec2 pt = HMM_Vec2{ 0, 0 };
    for (const auto& p : points) {
        pt += p.XY;
    }
    pt *= 2.0 / (points.size() - 1.0);
    pt -= points[node].XY * (points.size() + 1.0) / (points.size() - 1.0);
    points[node] = HMM_Vec4{ pt.X, pt.Y, static_cast<float>(func(pt.X, pt.Y)), 0 };
}

void Simplex::reduce(int node) {
    if (node == -1) {
        return;
    }
    for (size_t i=0; i < points.size(); i++) {
        if (static_cast<int>(i) == node) {
            continue;
        }
        HMM_Vec2 pt = (points[i].XY + points[node].XY) / 2.0;
        points[i] = HMM_Vec4{ pt.X, pt.Y, static_cast<float>(func(pt.X, pt.Y)), 0 };
    }
    max_node = -1;
}
