// Simplex.h
#ifndef SIMPLEX_H
#define SIMPLEX_H

static const int SimplexOrder = 3;

struct SIMPLEX_POINT {
	float x, y, z;
	float padding;
};

struct SIMPLEX {
    SIMPLEX_POINT points[SimplexOrder];
    int max_node;

	void set(float x1, float y1, float x2, float y2, float x3, float y3);
	void setTriangle(float size, float x, float y);

	int getMaxNode();
	int getMinNode();
	float getSize();
	SIMPLEX_POINT getMinPoint();

	void flip(int node);
	void reduce(int node);
};

/* void set_simplex(SIMPLEX &s,
                 double x1, double y1,
                 double x2, double y2,
                 double x3, double y3);
void set_triangle_simplex(SIMPLEX &s, double side, double x, double y);
int simplex_max_node(SIMPLEX s);
void simplex_flip(SIMPLEX &s, int node);
void simplex_reduce(SIMPLEX &s, int node);
double simplex_size(SIMPLEX s);
SIMPLEX_POINT simplex_min(SIMPLEX s); */

#endif
// SIMPLEX_H
