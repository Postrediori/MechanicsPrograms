#include "pch.h"
#include "MathUtils.h"
#include "FuncUtils.h"
#include "GraphUtils.h"
#include "SearchEngine.h"
#include "RelaxationEngine.h"

const ByteColor LineColor = {0, 0, 0, 0xff};
const ByteColor PreviousMarkerColor = {128, 64, 0, 0xff};
const ByteColor CurrentMarkerColor = {0, 64, 128, 0xff};

RelaxationEngine::RelaxationEngine() {
    set_start_point(XMin, YMin);
}

void RelaxationEngine::draw(CoordinateFunc xFunc, CoordinateFunc yFunc) {
    constexpr double PointSize = 0.1;
    constexpr int LineWidth = 3;

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    float oldW{ 0.0f };
    glGetFloatv(GL_LINE_WIDTH, &oldW);
    glLineWidth(LineWidth);
    glColor4ubv(LineColor.data());
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_line_style(FL_SOLID, LineWidth);
    fl_color(fl_rgb_color(LineColor[0], LineColor[1], LineColor[2]));
#endif

    DrawLine(
        xFunc(xold_), yFunc(yold_),
        xFunc(x_), yFunc(y_));

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glLineWidth(oldW);
    glColor4ubv(PreviousMarkerColor.data());
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(fl_rgb_color(PreviousMarkerColor[0], PreviousMarkerColor[1], PreviousMarkerColor[2]));
#endif
    DrawRectangle(
        xFunc(xold_ - PointSize), yFunc(yold_ - PointSize),
        xFunc(xold_ + PointSize), yFunc(yold_ + PointSize));

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glColor4ubv(CurrentMarkerColor.data());
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(fl_rgb_color(CurrentMarkerColor[0], CurrentMarkerColor[1], CurrentMarkerColor[2]));
#endif
    DrawRectangle(
        xFunc(x_ - PointSize), yFunc(y_ - PointSize),
        xFunc(x_ + PointSize), yFunc(y_ + PointSize));
}

void RelaxationEngine::search_start() {
    xlen_ = XMax - XMin;
    ylen_ = YMax - YMin;
    sx_ = 2.0 * xlen_ / Epsilon - 1.0;
    sy_ = 2.0 * ylen_ / Epsilon - 1.0;
    ddx_ = xlen_ / sx_;
    ddy_ = ylen_ / sy_;
    currentVar_ = 0;

    x_ = xold_ = xmin_ = xstart_;
    y_ = yold_ = ymin_ = ystart_;
    zmin_ = func(xmin_, ymin_);
    count_ = 1;

    search_over_ = false;
}

void RelaxationEngine::search_step() {
    if (search_over_) {
        return;
    }

    // Вычисление частных производных в точке (x,y)
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
    if (fabs(partialX) > fabs(partialY) && currentVar_ != 1) {
        currentVar_ = 1; // x
    }
    if (fabs(partialY) > fabs(partialX) && currentVar_ != 2) {
        currentVar_ = 2; // y
    }

    // Установка начальных значений для сканирования
    if (currentVar_==1) {
        dx_ = -ddx_ * signum(partialX);
        dy_ = 0;
    }
    else if (currentVar_==2) {
        dx_ = 0;
        dy_ = -ddy_ * signum(partialY);
    }

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
        x1 += dx_;
        y1 += dy_;
    }

    // Сравнение полученного минимума в направлении с уже известным минимумом
    if (zmin1 < zmin_) {
        xold_ = xmin_;
        yold_ = ymin_;
        x_ = xmin_ = xmin1;
        y_ = ymin_ = ymin1;
        zmin_ = zmin1;
    }
    else {
        // Изменение параметров поиска, если шаг неудачный
        ddx_ *= -0.5;
        ddy_ *= -0.5;
    }
}
