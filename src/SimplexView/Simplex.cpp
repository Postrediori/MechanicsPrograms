#include "pch.h"
#include "MathUtils.h"
#include "FuncUtils.h"
#include "Simplex.h"

void Simplex::set(float x1, float y1, float x2, float y2, float x3, float y3) {
    points[0] = { x1, y1, static_cast<float>(func(x1, y1)), 0.0f };
    points[1] = { x2, y2, static_cast<float>(func(x2, y2)), 0.0f };
    points[2] = { x3, y3, static_cast<float>(func(x3, y3)), 0.0f };
    max_node = -1;
}

void Simplex::setTriangle(float size, float x, float y) {
    for (int i=0; i<SimplexOrder; i++) {
        double angle = i * 2 * M_PI / SimplexOrder;
        points[i].x = x + size * cos(angle);
        points[i].y = y + size * sin(angle);
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
    for (int i=0; i<points.size(); i++) {
        float dx = points[i].x - points[(i + 1) % points.size()].x;
        float dy = points[i].y - points[(i + 1) % points.size()].y;
        sum += sqrt(dx * dx + dy * dy);
    }
    sum /= float(points.size());
    return sum;
}

vec4 Simplex::getMinPoint() const {
    return points.at(getMinNode());
}

void Simplex::flip(int node) {
    vec4 pt;
    for (const auto& p : points) {
        pt.x += p.x;
        pt.y += p.y;
    }
    pt.x *= 2.f / (points.size() - 1.f);
    pt.y *= 2.f / (points.size() - 1.f);
    pt.x -= points[node].x * (points.size() + 1.f) / (points.size() - 1.f);
    pt.y -= points[node].y * (points.size() + 1.f) / (points.size() - 1.f);
    pt.z = func(pt.x, pt.y);
    points[node] = pt;
}

void Simplex::reduce(int node) {
    vec4 pt;
    for (int i=0; i < points.size(); i++) {
        if (i == node) {
            continue;
        }
        pt = {
            (points[i].x + points[node].x) / 2.f,
            (points[i].y + points[node].y) / 2.f,
            static_cast<float>(func(pt.x, pt.y)),
            0.0f
        };
        points[i] = pt;
    }
    max_node = -1;
}
