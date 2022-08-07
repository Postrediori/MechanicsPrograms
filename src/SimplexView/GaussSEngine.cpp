#include "pch.h"
#include "MathUtils.h"
#include "FuncUtils.h"
#include "GraphUtils.h"
#include "SearchEngine.h"
#include "GaussSEngine.h"

GaussSEngine::GaussSEngine() {
    set_start_point(XMin, YMin);
}

void GaussSEngine::draw(CoordinateFunc xfunc, CoordinateFunc yfunc) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    float w{ 0.0f };
    glGetFloatv(GL_LINE_WIDTH, &w);
    glLineWidth(3.0);
    glColor3f(0.0, 0.0, 0.0);
    DrawLine(xold, yold, x, y);
    glLineWidth(w);

    glColor3f(0.5, 0.25, 0.0);
    DrawRectangle(xold-0.1, yold-0.1, xold+0.1, yold+0.1);

    glColor3f(0.0, 0.25, 0.5);
    DrawRectangle(x-0.1, y-0.1, x+0.1, y+0.1);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    //
#endif
}

void GaussSEngine::search_start() {
    ddx = 10.0 * Epsilon;
    ddy = 10.0 * Epsilon;

    x = xold = xmin_ = xstart_;
    y = yold = ymin_ = ystart_;
    zmin_ = func(xmin_, ymin_);
    count_ = 1;

    // Сначала поиск ведется по x
    currentVar = 1;

    search_over_ = false;
}

void GaussSEngine::search_step() {
    if (search_over_) {
        return;
    }
    double dx{ 0.0 }, dy{ 0.0 };
    double x1{ 0.0 }, y1{ 0.0 }, z1{ 0.0 };
    double x2{ 0.0 }, y2{ 0.0 }, z2{ 0.0 };
    double px{ 0.0 }, py{ 0.0 };

    // Установка приращений в зависимости от текущей переменной
    if (currentVar==1) {
        dx = ddx;
        dy = 0;
    }
    else if (currentVar==2) {
        dx = 0;
        dy = ddy;
    }

    // Начало работы метода тяжелого шарика
    x1 = x;
    y1 = y;
    z1 = func(x1, y1);
    x2 = x1 + dx;
    y2 = y1 + dy;
    z2 = func(x2, y2);
    count_ += 2;

    while (z1>z2) {
        if (currentVar==1) {
            // px = x2 - dx * (z2 - z1) / (x2 - x1) + dx * (x2 - x1);
            px = x2 - dx * (z2 - z1);
            py = y2;
        } else if (currentVar==2) {
            px = x2;
            // py = y2 - dy * (z2 - z1) / (y2 - y1) + dy * (y2 - y1);
            py = y2 - dy * (z2 - z1);
        }

        // Если выход за пределы области
        if (px>XMax || py>YMax) {
            x1 = XMax;
            y1 = YMax;
            z1 = func(x1, y1);
            count_++;
            search_over_ = true;

            xold = xmin_;
            yold = ymin_;
            return;
        }

        x1 = x2;
        y1 = y2;
        z1 = z2;
        x2 = px;
        y2 = py;
        z2 = func(x2, y2);
        count_++;
    }

    search_over_ = almost_equal(z1, zmin_);
    if (z1<zmin_) {
        xold = xmin_;
        yold = ymin_;
        x = xmin_ = x1;
        y = ymin_ = y1;
        zmin_ = z1;
    }

    if (search_over_) {
        xold = xmin_;
        yold = ymin_;
    }

    currentVar = currentVar % 2 + 1;
}
