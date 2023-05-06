#include "pch.h"
#include "GeometryUtils.h"
#include "OptimizationModel.h"
#include "GraphWidget.h"

GraphWidget::GraphWidget(int x, int y, int w, int h, const char *lbl)
    : Fl_Widget(x, y, w, h, lbl) {
    update_offscreen();

    plotLines_.resize(PlotPointsCount - 1);
    ticksX_.resize(TickCount + 1);
    ticksY_.resize(TickCount + 1);

    coordFunc_ = [=](Point p) -> Point {
        return {
            (p.x - area_.xmin) / pixelX_ + Margin + TickSize,
            (area_.ymax - p.y) / pixelY_ + Margin
        };
    };
}

GraphWidget::~GraphWidget() {
    release_offscreen();
}

void GraphWidget::release_offscreen() {
    if (offscreenCreated_) {
        fl_delete_offscreen(offscreen_);
    }
    offscreenCreated_ = false;
}

void GraphWidget::resize(int x, int y, int w, int h) {
    Fl_Widget::resize(x, y, w, h);

    update_offscreen();
    update_scale();
}

void GraphWidget::draw() {
    fl_begin_offscreen(offscreen_);

    // Clear screen
    fl_color(fl_rgb_color(255));
    fl_rectf(0, 0, this->w(), this->h());

    //
    // Draw axis and ticks
    //

    // Draw box
    fl_color(fl_rgb_color(0));
    fl_line_style(FL_SOLID, 1);

    auto p1 = coordFunc_({ area_.xmin, area_.ymin });
    auto p2 = coordFunc_({ area_.xmax, area_.ymin });
    auto p3 = coordFunc_({ area_.xmax, area_.ymax });
    auto p4 = coordFunc_({ area_.xmin, area_.ymax });
    fl_loop(p1.x, p1.y,
        p2.x, p2.y,
        p3.x, p3.y,
        p4.x, p4.y);

    // Draw ticks
    fl_color(fl_rgb_color(0));
    fl_line_style(FL_SOLID, 1);

    for (const auto& t : ticksX_) {
        auto p1 = coordFunc_(t.p1);
        auto p2 = coordFunc_(t.p2);
        fl_line(p1.x, p1.y, p2.x, p2.y);
    }

    for (const auto& t : ticksY_) {
        auto p1 = coordFunc_(t.p1);
        auto p2 = coordFunc_(t.p2);
        fl_line(p1.x, p1.y, p2.x, p2.y);
    }

    // Print plot ranges
    fl_color(fl_rgb_color(0));
    fl_font(FL_HELVETICA, 14);

    std::stringstream s;

    s << std::fixed << std::setprecision(1) << area_.xmin;
    fl_draw(s.str().c_str(),
        p1.x, p1.y + TickSize,
        0, 0, FL_ALIGN_TOP | FL_ALIGN_LEFT);

    s.str(std::string());
    s << std::fixed << std::setprecision(1) << area_.xmax;
    fl_draw(s.str().c_str(),
        p2.x, p2.y + TickSize,
        0, 0, FL_ALIGN_TOP | FL_ALIGN_RIGHT);

    s.str(std::string());
    s << std::fixed << std::setprecision(1) << area_.ymin;
    fl_draw(s.str().c_str(),
        p1.x, p1.y,
        0, 0, FL_ALIGN_BOTTOM | FL_ALIGN_RIGHT);

    s.str(std::string());
    s << std::fixed << std::setprecision(1) << area_.ymax;
    fl_draw(s.str().c_str(),
        p4.x, p4.y,
        0, 0, FL_ALIGN_TOP | FL_ALIGN_RIGHT);

    //
    // Draw plot
    //
    fl_color(fl_rgb_color(255, 25, 50));
    fl_line_style(FL_SOLID, 1);
    for (const auto& pl : plotLines_) {
        auto p1 = coordFunc_(pl.p1);
        auto p2 = coordFunc_(pl.p2);
        fl_line(p1.x, p1.y, p2.x, p2.y);
    }

    // Draw current model status
    if (model_) {
        model_->draw(coordFunc_);
    }

    fl_end_offscreen();

    fl_copy_offscreen(this->x(), this->y(), this->w(), this->h(),
        offscreen_, 0, 0);
}

void GraphWidget::set_plot(Function1D func, Area functionArea) {
    area_ = functionArea;

    update_scale();
    update_plot(func);
    update_ticks();
}

void GraphWidget::update_offscreen() {
    release_offscreen();

    offscreen_ = fl_create_offscreen(this->w(), this->h());
    offscreenCreated_ = true;
}

void GraphWidget::update_scale() {
    pixelX_ = (area_.xmax - area_.xmin) / (this->w() - Margin * 2 - TickSize);
    pixelY_ = (area_.ymax - area_.ymin) / (this->h() - Margin * 2 - TickSize);
}

void GraphWidget::update_plot(Function1D func) {
    double dx = (area_.xmax - area_.xmin) / static_cast<double>(PlotPointsCount - 1);
    for (int i = 0; i<PlotPointsCount - 1; i++) {
        double x1 = area_.xmin + static_cast<double>(i) * dx;
        double x2 = area_.xmin + static_cast<double>(i + 1) * dx;
        plotLines_[i] = {
            { x1, func(x1) },
            { x2, func(x2) },
        };
    }
}

void GraphWidget::update_ticks() {
    // Y Ticks
    double dy = (area_.ymax - area_.ymin) / static_cast<double>(TickCount);
    for (int i=0; i<=TickCount; i++) {
        double y = area_.ymin + i * dy;
        double tickScale = (i % 2) ? 0.5 : 1.0;

        ticksY_[i] = {
            {area_.xmin, y},
            {area_.xmin - TickSize * pixelX_ * tickScale, y }
        };
    }

    // X Ticks
    double dx = (area_.xmax - area_.xmin) / static_cast<double>(TickCount);
    for (int i=0; i<=TickCount; i++) {
        double x = area_.xmin + i * dx;
        double tickScale = (i % 2) ? 0.5 : 1.0;

        ticksX_[i] = {
            {x, area_.ymin},
            {x, area_.ymin - TickSize * pixelY_ * tickScale}
        };
    }
}
