#include "pch.h"
#include "MathUtils.h"
#include "FuncUtils.h"
#include "GraphUtils.h"
#include "SearchEngine.h"
#include "ScanEngine.h"

const ByteColor MarkerColor = {0, 64, 128, 0xff};

ScanEngine::ScanEngine() {
    search_start();
}

void ScanEngine::draw(CoordinateFunc xFunc, CoordinateFunc yFunc) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glColor4ubf(MarkerColor.data());
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(fl_rgb_color(MarkerColor[0], MarkerColor[1], MarkerColor[2]));
#endif
    constexpr double PointSize = 0.1;
    DrawRectangle(
        xFunc(xmin_ - PointSize), yFunc(ymin_ - PointSize),
        xFunc(xmin_ + PointSize), yFunc(ymin_ + PointSize));
}

void ScanEngine::search_start() {
    double xlen = XMax - XMin;
    double ylen = YMax - YMin;
    double sx = 2 * xlen / Epsilon - 1;
    double sy = 2 * ylen / Epsilon - 1;
    dx_ = xlen / sx;
    dy_ = ylen / sy;

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

    for (double x = XMin; x <= XMax; x += dx_) {
        for (double y = YMin; y <= YMax; y += dy_) {
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
