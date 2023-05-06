#pragma once

struct Point {
    double x;
    double y;
};

struct Line {
    Point p1;
    Point p2;
};

struct Area {
    double xmin;
    double xmax;
    double ymin;
    double ymax;
};

// Pointer to one-dimensional function
using Function1D = std::function<double(double)>;

// Function that transforms one coordinate system into some other
using CoordinateFunction = std::function<Point(Point)>;
