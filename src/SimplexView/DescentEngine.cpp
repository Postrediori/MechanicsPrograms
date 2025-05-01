#include "pch.h"
#include "MathUtils.h"
#include "FuncUtils.h"
#include "GraphUtils.h"
#include "SearchEngine.h"
#include "DescentEngine.h"

const Fl_Color LineColor = fl_rgb_color(0);
const Fl_Color PreviousMarkerColor = fl_rgb_color(128, 64, 0);
const Fl_Color CurrentMarkerColor = fl_rgb_color(0, 64, 128);

DescentEngine::DescentEngine() {
    set_start_point(XMin, YMin);
}

void DescentEngine::draw(CoordinateFunc xFunc, CoordinateFunc yFunc) {
    constexpr double PointSize = 0.1;
    constexpr int LineWidth = 3;

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    float oldW{ 0.0f };
    glGetFloatv(GL_LINE_WIDTH, &oldW);
    glLineWidth(LineWidth);
    SET_FL_COLOR_TO_GL(LineColor);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_line_style(FL_SOLID, LineWidth);
    fl_color(LineColor);
#endif

    DrawLine(
        xFunc(xold_), yFunc(yold_),
        xFunc(x_), yFunc(y_));

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glLineWidth(oldW);
    SET_FL_COLOR_TO_GL(PreviousMarkerColor);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(PreviousMarkerColor);
#endif
    DrawRectangle(
        xFunc(xold_ - PointSize), yFunc(yold_ - PointSize),
        xFunc(xold_ + PointSize), yFunc(yold_ + PointSize));

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    SET_FL_COLOR_TO_GL(CurrentMarkerColor);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(CurrentMarkerColor);
#endif
    DrawRectangle(
        xFunc(x_ - PointSize), yFunc(y_ - PointSize),
        xFunc(x_ + PointSize), yFunc(y_ + PointSize));
}

void DescentEngine::search_start() {
    double xlen = XMax - XMin;
    double ylen = YMax - YMin;
    double sx = 2 * xlen / Epsilon - 1;
    double sy = 2 * ylen / Epsilon - 1;
    ddx_ = xlen / sx;
    ddy_ = ylen / sy;

    x_ = xold_ = xmin_ = xstart_;
    y_ = yold_ = ymin_ = ystart_;
    zmin_ = func(xmin_, ymin_);
    count_ = 1;

    search_over_ = false;
}

void DescentEngine::search_step() {
    if (search_over_) {
        return;
    }

    // Вычисление градиента в точке (x,y)
    double partialX = dfdx(x_, y_);
    double partialY = dfdy(x_, y_);
    count_ += 4;

    // Проверка условия выхода
    double partial = sqrt(partialX * partialX + partialY * partialY);
    if (partial < Epsilon) {
        xold_ = xmin_;
        yold_ = ymin_;
        search_over_ = true;
        return;
    }

    // Определние направления поиска, в котором модуль производной наибольший
    // if (abs(partialX)>abs(partialY) && currentVar!=1) currentVar = 1; // x
    // if (abs(partialY)>abs(partialX) && currentVar!=2) currentVar = 2; // y

    // Установка начальных значений для сканирования
    double dx = -ddx_ * signum(partialX);
    double dy = -ddy_ * signum(partialY);

    double x1 = x_, xmin1 = x_;
    double y1 = y_, ymin1 = y_;
    double zmin1 = func(x1, y1);

    // Сканирование в направлении
    while (x1>=XMin && x1<=XMax && y1>=YMin && y1<=YMax) {
        double z1 = func(x1, y1);
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
        xold_ = xmin_;
        yold_ = ymin_;
        x_ = xmin_ = xmin1;
        y_ = ymin_ = ymin1;
        zmin_ = zmin1;
    } else {
        // Изменение параметров поиска, если шаг неудачный
        ddx_ *= -0.5;
        ddy_ *= -0.5;
    }
}
