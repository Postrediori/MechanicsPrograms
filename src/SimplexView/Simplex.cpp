// Simplex.cpp

#include <cmath>
#include "Func.h"
#include "Simplex.h"

void SIMPLEX::set(float x1, float y1, float x2, float y2, float x3, float y3) {
    points[0].x = x1;
    points[0].y = y1;
	points[0].z = func(x1, y1);
    points[1].x = x2;
    points[1].y = y2;
	points[1].z = func(x2, y2);
    points[2].x = x3;
    points[2].y = y3;
	points[2].z = func(x3, y3);
    max_node    = -1;
}

void SIMPLEX::setTriangle(float size, float x, float y) {
	for (int i=0; i<SimplexOrder; i++) {
		double angle = i * 2 * M_PI / SimplexOrder;
		points[i].x = x + size * cos(angle);
		points[i].y = y + size * sin(angle);
	}
    max_node = -1;
}

int SIMPLEX::getMaxNode() {
    int mx_node = 0;
    float zmax = points[mx_node].z; //func(s.points[max_node].x, s.points[max_node].y);
	for (int i=1; i<SimplexOrder; i++) {
        double z = points[i].z;
        if (z>zmax) {
            zmax = z;
            mx_node = i;
        }
    }
    return mx_node;
}

int SIMPLEX::getMinNode() {
    int mn_node = 0;
    float zmin = points[mn_node].z; //func(s.points[max_node].x, s.points[max_node].y);
	for (int i=1; i<SimplexOrder; i++) {
        float z = points[i].z;
        if (z<zmin) {
            zmin = z;
            mn_node = i;
        }
    }
    return mn_node;
}

float SIMPLEX::getSize() {
    float sum = 0.f;
    for (int i=0; i<SimplexOrder; i++) {
        float dx = points[i].x - points[(i + 1) % SimplexOrder].x;
        float dy = points[i].y - points[(i + 1) % SimplexOrder].y;
        sum += sqrt(dx * dx + dy * dy);
    }
    sum /= float(SimplexOrder);
    return sum;
}

SIMPLEX_POINT SIMPLEX::getMinPoint() {
	return points[getMinNode()];
}

void SIMPLEX::flip(int node) {
    SIMPLEX_POINT pt = {0.f, 0.f, 0.f};
    for (int i=0; i<SimplexOrder; i++) {
        pt.x += points[i].x;
        pt.y += points[i].y;
    }
    pt.x *= 2.f / (SimplexOrder - 1.f);
    pt.y *= 2.f / (SimplexOrder - 1.f);
    pt.x -= points[node].x * (SimplexOrder + 1.f) / (SimplexOrder - 1.f);
    pt.y -= points[node].y * (SimplexOrder + 1.f) / (SimplexOrder - 1.f);
	pt.z = func(pt.x, pt.y);
    points[node] = pt;
}

void SIMPLEX::reduce(int node) {
    SIMPLEX_POINT pt = {0.f, 0.f, 0.f};
    for (int i=0; i<SimplexOrder; i++) {
        if (i==node) continue;
        pt.x = (points[i].x + points[node].x) / 2.f;
        pt.y = (points[i].y + points[node].y) / 2.f;
        pt.z = func(pt.x, pt.y);
        points[i] = pt;
    }
    max_node = -1;
}

/* void simplex_reduce(SIMPLEX &s, int node) {
    SIMPLEX_POINT pt = {0.0, 0.0, 0.0};
    for (int i=0; i<SimplexDgt; i++) {
        pt.x += s.points[i].x;
        pt.y += s.points[i].y;
    }
    pt.x *= 3.0 / (2.0 * (SimplexDgt - 1.0));
    pt.y *= 3.0 / (2.0 * (SimplexDgt - 1.0));
    pt.x -= s.points[node].x * (SimplexDgt + 2.0) / (2.0 * (SimplexDgt - 1.0));
    pt.y -= s.points[node].y * (SimplexDgt + 2.0) / (2.0 * (SimplexDgt - 1.0));
    s.points[node] = pt;
    s.max_node = -1;
} */
