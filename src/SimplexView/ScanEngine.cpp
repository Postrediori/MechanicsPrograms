#include "pch.h"
#include "MathUtils.h"
#include "FuncUtils.h"
#include "GraphUtils.h"
#include "SearchEngine.h"
#include "ScanEngine.h"

ScanEngine::ScanEngine() {
    search_start();
}

void ScanEngine::draw(CoordinateFunc xfunc, CoordinateFunc yfunc) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glColor3f(0.0, 0.25, 0.5);
    DrawRectangle(xmin_-0.1, ymin_-0.1, xmin_+0.1, ymin_+0.1);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    //
#endif
}

void ScanEngine::search_start() {
    double xlen = XMax - XMin;
    double ylen = YMax - YMin;
    double sx = 2 * xlen / Epsilon - 1;
    double sy = 2 * ylen / Epsilon - 1;
    dx = xlen / sx;
    dy = ylen / sy;

    xmin_ = XMin;
    ymin_ = YMin;
    zmin_ = func(xmin_, ymin_);

    count_ = 1;
    search_over_ = false;
}

void ScanEngine::search_step() {
    if (search_over_) {
        return;
    }

    for (double x=XMin; x<=XMax; x+=dx) {
        for (double y=YMin; y<=YMax; y+=dy) {
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
