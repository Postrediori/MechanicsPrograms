#include "pch.h"
#include "MathUtils.h"
#include "FuncUtils.h"
#include "GraphUtils.h"
#include "SearchEngine.h"
#include "DescentEngine.h"

DescentEngine::DescentEngine() {
    set_start_point(XMin, YMin);
}

void DescentEngine::draw(CoordinateFunc xfunc, CoordinateFunc yfunc) {
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

void DescentEngine::search_start() {
    double xlen = XMax - XMin;
    double ylen = YMax - YMin;
    double sx = 2 * xlen / Epsilon - 1;
    double sy = 2 * ylen / Epsilon - 1;
    ddx = xlen / sx;
    ddy = ylen / sy;

    x = xold = xmin_ = xstart_;
    y = yold = ymin_ = ystart_;
    zmin_ = func(xmin_, ymin_);
    count_ = 1;
}

void DescentEngine::search_step() {
    if (search_over_) {
        return;
    }

    // Вычисление градиента в точке (x,y)
    double partialX = dfdx(x, y);
    double partialY = dfdy(x, y);
    count_ += 4;

    // Проверка условия выхода
    double partial = sqrt(partialX * partialX + partialY * partialY);
    if (partial < Epsilon) {
        xold = xmin_;
        yold = ymin_;
        search_over_ = true;
        return;
    }

    // Определние направления поиска, в котором модуль производной наибольший
    // if (abs(partialX)>abs(partialY) && currentVar!=1) currentVar = 1; // x
    // if (abs(partialY)>abs(partialX) && currentVar!=2) currentVar = 2; // y

    // Установка начальных значений для сканирования
    double dx = -ddx * signum(partialX);
    double dy = -ddy * signum(partialY);

    double x1, y1, z1, xmin1, ymin1, zmin1;
    x1 = xmin1 = x;
    y1 = ymin1 = y;
    zmin1 = func(x1, y1);

    // Сканирование в направлении
    while (x1>=XMin && x1<=XMax && y1>=YMin && y1<=YMax) {
        z1 = func(x1, y1);
        count_++;

        if (z1<zmin1) {
            xmin1 = x1;
            ymin1 = y1;
            zmin1 = z1;
        }
        x1 += dx;
        y1 += dy;
    }

    // Сравнение полученного минимума в направлении с уже известным минимумом
    if (zmin1<zmin_) {
        xold = xmin_;
        yold = ymin_;
        x = xmin_ = xmin1;
        y = ymin_ = ymin1;
        zmin_ = zmin1;
    } else {
        // Изменение параметров поиска, если шаг неудачный
        ddx *= -0.5;
        ddy *= -0.5;
    }
}
