// RelaxationEngine.cpp

#include <FL/gl.h>
#include <math.h>
#include "Func.h"
#include "Graph.h"
#include "RelaxationEngine.h"

RelaxationEngine::RelaxationEngine() {
    set_start_point(XMin, YMin);
}

void RelaxationEngine::draw() {
    float w;
    glGetFloatv(GL_LINE_WIDTH, &w);
    glLineWidth(3.0);
    glColor3f(0.0, 0.0, 0.0);
    DrawLine(xold, yold, x, y);
    glLineWidth(w);

    glColor3f(0.5, 0.25, 0.0);
    DrawRectangle(xold-0.1, yold-0.1, xold+0.1, yold+0.1);

    glColor3f(0.0, 0.25, 0.5);
    DrawRectangle(x-0.1, y-0.1, x+0.1, y+0.1);
}

void RelaxationEngine::search_start() {
    xlen = XMax - XMin;
    ylen = YMax - YMin;
    sx = 2 * xlen / Epsilon - 1;
    sy = 2 * ylen / Epsilon - 1;
    ddx = xlen / sx;
    ddy = ylen / sy;
    currentVar = 0;

    x = xold = xmin_ = xstart_;
    y = yold = ymin_ = ystart_;
    zmin_ = func(xmin_, ymin_);
    count_ = 1;
}

void RelaxationEngine::search_step() {
    // Вычисление частных производных в точке (x,y)
    double partialX = (func(x + Epsilon, y) - func(x, y)) / Epsilon;
    double partialY = (func(x, y + Epsilon) - func(x, y)) / Epsilon;
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
    if (fabs(partialX)>fabs(partialY) && currentVar!=1) currentVar = 1; // x
    if (fabs(partialY)>fabs(partialX) && currentVar!=2) currentVar = 2; // y

    // Установка начальных значений для сканирования
    if (currentVar==1) {
        dx = -ddx * signum(partialX);
        dy = 0;
    } else if (currentVar==2) {
        dx = 0;
        dy = -ddy * signum(partialY);
    }

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
