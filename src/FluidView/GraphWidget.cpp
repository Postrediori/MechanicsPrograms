#include "pch.h"
#include "FluidModel.h"
#include "GraphWidget.h"

const double XMin = 0.0;
const double XMax = MaxLambda;
const double YMin = 0.0;
const double YMax = 1.0;

const int Margin = 20;
const int TickSize = 10;

const Fl_Color BackgroundColor = fl_rgb_color(255);
const Fl_Color LineColor = fl_rgb_color(0);
const Fl_Color PlotColor = fl_rgb_color(255, 25, 50);
const Fl_Color CurrentValueColor = fl_rgb_color(32, 128, 32);
const Fl_Color CurrentSolutionsColor = fl_rgb_color(0, 64, 192);

GraphWidget::GraphWidget(int X, int Y, int W, int H, const char* l)
    : Fl_Widget(X, Y, W, H, l) {
    this->lines_ = false;

    this->pixelX_ = (XMax - XMin) / (this->w() - Margin * 2 - TickSize);
    this->pixelY_ = (YMax - YMin) / (this->h() - Margin * 2 - TickSize);

    double dx = (XMax - XMin) / (double)(PointCount);
    for (int i = 0; i < PointCount; i++) {
        // double x = i / 1000.0 - 2.0;
        double x = XMin + i * dx;
        this->graph_[i].x = x;
        this->graph_[i].y = qfunc(x);
    }

    // Y Ticks
    double dTick = (YMax - YMin) / TickCount;
    for (int i = 0; i <= TickCount; i++) {
        double y = YMin + i * dTick;
        ticksY[i * 2].x = XMin;
        ticksY[i * 2].y = y;

        double tickScale = (i % 2) ? 0.5 : 1.0;
        ticksY[i * 2 + 1].x = XMin - TickSize * this->pixelX_ * tickScale;
        ticksY[i * 2 + 1].y = y;
    }

    // X Ticks
    dTick = (XMax - XMin) / TickCount;
    for (int i = 0; i <= TickCount; i++) {
        double x = XMin + i * dTick;
        ticksX[i * 2].y = YMin;
        ticksX[i * 2].x = x;

        double tickScale = (i % 2) ? 0.5 : 1.0;
        ticksX[i * 2 + 1].y = YMin - TickSize * this->pixelY_ * tickScale;
        ticksX[i * 2 + 1].x = x;
    }
}

void GraphWidget::draw() {
    fl_draw_box(FL_FLAT_BOX,
        this->x(), this->y(),
        this->w(), this->h(),
        BackgroundColor);

    //
    // Draw axis and ticks
    //
#define GET_X(xx) ((xx-XMin)/this->pixelX_+Margin+TickSize+this->x())
#define GET_Y(yy) ((YMax-yy)/this->pixelY_+Margin+this->y())

    // Draw box
    fl_color(LineColor);
    fl_line_style(FL_SOLID, 1, nullptr);

    fl_loop(GET_X(XMin), GET_Y(YMin),
        GET_X(XMax), GET_Y(YMin),
        GET_X(XMax), GET_Y(YMax),
        GET_X(XMin), GET_Y(YMax));

    // Draw ticks
    for (int i = 0; i <= TickCount; i++) {
        fl_line(GET_X(ticksX[i * 2].x), GET_Y(ticksX[i * 2].y),
            GET_X(ticksX[i * 2 + 1].x), GET_Y(ticksX[i * 2 + 1].y));

        fl_line(GET_X(ticksY[i * 2].x), GET_Y(ticksY[i * 2].y),
            GET_X(ticksY[i * 2 + 1].x), GET_Y(ticksY[i * 2 + 1].y));
    }

    // Print plot ranges
    char str[32];

    snprintf(str, sizeof(str), "%.1f", XMin);
    fl_draw(str, GET_X(XMin), GET_Y(YMin + this->pixelY_ * (Margin + TickSize)));

    snprintf(str, sizeof(str), "%.1f", XMax);
    fl_draw(str, GET_X(XMax), GET_Y(YMin + this->pixelY_ * (Margin + TickSize)));

    snprintf(str, sizeof(str), "%.1f", YMax);
    fl_draw(str, GET_X(XMin - this->pixelX_ * (Margin + TickSize)),
        GET_Y(YMax + this->pixelY_ * (Margin / 2)));

    snprintf(str, sizeof(str), "%.1f", YMin);
    fl_draw(str, GET_X(XMin - this->pixelX_ * (Margin + TickSize)),
        GET_Y(YMin - this->pixelY_ * (Margin / 2)));

    //
    // Draw graph
    //
    fl_color(PlotColor);
    for (int i = 0; i < PointCount - 1; i++) {
        fl_line(GET_X(this->graph_[i].x), GET_Y(this->graph_[i].y),
            GET_X(this->graph_[i + 1].x), GET_Y(this->graph_[i + 1].y));
    }

    if (this->lines_) {
        fl_color(CurrentValueColor);
        fl_line(GET_X(XMin), GET_Y(this->q_),
            GET_X(XMax), GET_Y(this->q_));

        fl_color(CurrentSolutionsColor);

        fl_line(GET_X(this->lambda1_), GET_Y(YMin),
            GET_X(this->lambda1_), GET_Y(this->q_));

        fl_line(GET_X(this->lambda2_), GET_Y(YMin),
            GET_X(this->lambda2_), GET_Y(this->q_));
    }
}

void GraphWidget::set_lines(float q, float lambda1, float lambda2) {
    if (!this->lines_) this->lines_ = true;
    this->q_ = q;
    this->lambda1_ = lambda1;
    this->lambda2_ = lambda2;
}
