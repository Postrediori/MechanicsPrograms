// ScanEngine.cpp

#include <FL/gl.h>
#include "Func.h"
#include "Graph.h"
#include "ScanEngine.h"

ScanEngine::ScanEngine() {
    search_start();
}

void ScanEngine::draw() {
    glColor3f(0.0, 0.25, 0.5);
    DrawRectangle(xmin_-0.1, ymin_-0.1, xmin_+0.1, ymin_+0.1);
}

void ScanEngine::search_start() {
    double xlen, ylen;
    double sx, sy;
    xlen = XMax - XMin;
    ylen = YMax - YMin;
    sx = 2 * xlen / Epsilon - 1;
    sy = 2 * ylen / Epsilon - 1;
    dx = xlen / sx;
    dy = ylen / sy;

    xmin_ = XMin;
    ymin_ = YMin;
    zmin_ = func(xmin_, ymin_);

    count_ = 1;
    search_over_ = false;
}

void ScanEngine::search_step() {
    if (search_over_) return;

    double x, y;
    for (x=XMin; x<=XMax; x+=dx) {
        for (y=YMin; y<=YMax; y+=dy) {
            double z = func(x,y);
            count_++;

            if (z<zmin_) {
                zmin_ = z;
                xmin_ = x;
                ymin_ = y;
            }
        }
    }

    search_over_ = true;
}
