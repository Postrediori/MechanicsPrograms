#include "pch.h"
#include "MathUtils.h"
#include "FuncUtils.h"
#include "GraphUtils.h"
#include "SearchEngine.h"
#include "GaussSEngine.h"

const Fl_Color LineColor = fl_rgb_color(0);
const Fl_Color PreviousMarkerColor = fl_rgb_color(128, 64, 0);
const Fl_Color CurrentMarkerColor = fl_rgb_color(0, 64, 128);

GaussSEngine::GaussSEngine() {
    set_start_point(XMin, YMin);
}

void GaussSEngine::draw(CoordinateFunc xFunc, CoordinateFunc yFunc) {
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

void GaussSEngine::search_start() {
    ddx_ = 10.0 * Epsilon;
    ddy_ = 10.0 * Epsilon;

    x_ = xold_ = xmin_ = xstart_;
    y_ = yold_ = ymin_ = ystart_;
    zmin_ = func(xmin_, ymin_);
    count_ = 1;

    // Сначала поиск ведется по x
    currentVar_ = 1;

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
    if (currentVar_==1) {
        dx = ddx_;
        dy = 0;
    }
    else if (currentVar_==2) {
        dx = 0;
        dy = ddy_;
    }

    // Начало работы метода тяжелого шарика
    x1 = x_;
    y1 = y_;
    z1 = func(x1, y1);

    x2 = x1 + dx;
    y2 = y1 + dy;
    z2 = func(x2, y2);

    count_ += 2;

    while (z1 > z2) {
        if (currentVar_==1) {
            // px = x2 - dx * (z2 - z1) / (x2 - x1) + dx * (x2 - x1);
            px = x2 - dx * (z2 - z1);
            py = y2;
        }
        else if (currentVar_==2) {
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

            xold_ = xmin_;
            yold_ = ymin_;
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
        xold_ = xmin_;
        yold_ = ymin_;
        x_ = xmin_ = x1;
        y_ = ymin_ = y1;
        zmin_ = z1;
    }

    if (search_over_) {
        xold_ = xmin_;
        yold_ = ymin_;
    }

    currentVar_ = currentVar_ ^ 3;
}
