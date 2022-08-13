#include "pch.h"
#include "MathUtils.h"
#include "FuncUtils.h"
#include "GraphUtils.h"
#include "SearchEngine.h"
#include "ContourPlot.h"
#include "GraphWidget.h"

namespace PlotParams {
    constexpr int XDivCount = 100;
    constexpr int YDivCount = 100;

    constexpr int TickCount = 20;
    constexpr int TickSize = 10;
    constexpr int Margin = 20;
}

const std::vector<ByteColor> Palette = {
    {0xf1, 0xed, 0xc8, 0x00},
    {0xe7, 0xdb, 0xa9, 0x00},
    {0xdd, 0xc9, 0x8a, 0x00},
    {0xba, 0xc5, 0x96, 0x00},
    {0xa4, 0xca, 0xa1, 0x00},
    {0xbf, 0xd5, 0xee, 0x00},
    {0xa3, 0xc3, 0xe6, 0x00},
    {0x89, 0xb0, 0xda, 0x00},
    {0x6b, 0x99, 0xc9, 0x00},
    {0x51, 0x82, 0xb4, 0x00},
};

//const std::vector<ByteColor> Palette = {
//    {0xe2, 0x08, 0x00, 0x00},
//    {0xf0, 0x71, 0x13, 0x00},
//    {0xff, 0xe4, 0x29, 0x00},
//    {0x91, 0xa8, 0x7b, 0x00},
//    {0x2c, 0x72, 0xc7, 0x00},
//    {0x17, 0x5b, 0xaa, 0x00},
//    {0x00, 0x38, 0x76, 0x00},
//    {0x00, 0x30, 0x65, 0x00},
//    {0x00, 0x27, 0x54, 0x00},
//    {0x00, 0x1e, 0x43, 0x00},
//};

const ByteColor ColorLevelLines = {0x55, 0x55, 0x55, 0xff};
const ByteColor ColorTicks = {0, 0, 0, 0xff};

GraphWidget::GraphWidget(int X, int Y, int W, int H, SearchEngine* e, const char* l)
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    : Fl_Gl_Window(X, Y, W, H, l)
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    : Fl_Widget(X, Y, W, H, l)
#endif
    , engine_(e) {
    using namespace PlotParams;

    boundingBox_ = {
        HMM_Vec2(XMin, YMin),
        HMM_Vec2(XMax, YMin),
        HMM_Vec2(XMax, YMax),
        HMM_Vec2(XMin, YMax)
    };

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    xFunc_ = [=](double x) -> double {
        return x;
    };
    yFunc_ = [=](double y) -> double {
        return y;
    };
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    xFunc_ = [=](double x) -> double {
        return (x - XMin) / this->pixelX_ + Margin + TickSize;
    };
    yFunc_ = [=](double y) -> double {
        return (YMax - y) / this->pixelY_ + Margin;
    };
#endif

    contourLines_.resize(Palette.size());
    contourFills_.resize(Palette.size());

    create_surface();

    update_size();
}

GraphWidget::~GraphWidget() {
#if DRAW_METHOD==DRAW_METHOD_FLTK
    if (initOffscreen_) {
        fl_delete_offscreen(offscreen_);
        fl_delete_offscreen(contourOfs_);
    }
#endif
}

void GraphWidget::resize(int x, int y, int w, int h) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    Fl_Gl_Window::resize(x, y, w, h);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    Fl_Widget::resize(x, y, w, h);
#endif
    update_size();
}

void GraphWidget::draw() {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    if (!this->valid()) {
        using namespace PlotParams;

        update_size();

        glViewport(0, 0, this->w(), this->h());
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(XMin - pixelX_*(Margin+TickSize),
                XMax + pixelX_*Margin,
                YMin - pixelY_*(Margin+TickSize),
                YMax + pixelY_*Margin);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
#endif

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    if (!this->context_valid()) {
        glShadeModel(GL_SMOOTH);
        glClearColor(1.0, 1.0, 1.0, 0.5);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }
#endif

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    draw_contour_plot();
    draw_contour_lines();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    if (contourOfsNeedsRedraw_) {
        fl_begin_offscreen(contourOfs_);

        fl_color(fl_rgb_color(255));
        fl_rectf(0, 0, this->w(), this->h());

        draw_contour_plot();
        draw_contour_lines();

        fl_end_offscreen();

        contourOfsNeedsRedraw_ = false;
    }

    fl_begin_offscreen(offscreen_);

    fl_copy_offscreen(0, 0, w(), h(), contourOfs_, 0, 0);
#endif

    draw_engine_status();

    draw_box();

    draw_ticks();

    draw_legend();

#if DRAW_METHOD==DRAW_METHOD_FLTK
    fl_end_offscreen();

    fl_copy_offscreen(this->x(), this->y(), this->w(), this->h(),
        offscreen_, 0, 0);
#endif
}

void GraphWidget::draw_contour_plot() {
    for (size_t i = 0; i < Palette.size(); i++) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
        glColor4ubv(Palette[i].data());
#elif DRAW_METHOD==DRAW_METHOD_FLTK
        fl_color(fl_rgb_color(Palette[i][0], Palette[i][1], Palette[i][2]));
#endif
        contourFills_[i].render(xFunc_, yFunc_);
    }
}

void GraphWidget::draw_contour_lines() {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glColor4ubv(ColorLevelLines.data());
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(fl_rgb_color(ColorLevelLines[0], ColorLevelLines[1], ColorLevelLines[2]));
#endif
    for (const auto& p : contourLines_) {
        p.render(xFunc_, yFunc_);
    }
}

void GraphWidget::draw_engine_status() {
    if (engine_) {
        engine_->draw(xFunc_, yFunc_);
    }
}

void GraphWidget::draw_box() {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glColor4ubv(ColorTicks.data());
    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, boundingBox_.data());
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(fl_rgb_color(ColorTicks[0], ColorTicks[1], ColorTicks[2]));
    fl_line_style(FL_SOLID, 1);

    fl_begin_loop();
    for (const auto& p : boundingBox_) {
        fl_vertex(xFunc_(p.X), yFunc_(p.Y));
    }
    fl_end_loop();
#endif
}

void GraphWidget::draw_ticks() {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    using namespace PlotParams;

    glColor4ubv(ColorTicks.data());
    glEnableClientState(GL_VERTEX_ARRAY);

    if (ticksX_.size() > 0) {
        glVertexPointer(2, GL_FLOAT, 0, ticksX_.data());
        glDrawArrays(GL_LINES, 0, (TickCount + 1) * 2);
    }

    if (ticksY_.size() > 0) {
        glVertexPointer(2, GL_FLOAT, 0, ticksY_.data());
        glDrawArrays(GL_LINES, 0, (TickCount + 1) * 2);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(fl_rgb_color(ColorTicks[0], ColorTicks[1], ColorTicks[2]));
    fl_line_style(FL_SOLID, 1);

    for (size_t i = 0; i < ticksX_.size(); i += 2) {
        const auto& p1 = ticksX_[i];
        const auto& p2 = ticksX_[i + 1];
        fl_line(xFunc_(p1.X), yFunc_(p1.Y),
            xFunc_(p2.X), yFunc_(p2.Y));
    }

    for (size_t i = 0; i < ticksY_.size(); i += 2) {
        const auto& p1 = ticksY_[i];
        const auto& p2 = ticksY_[i + 1];
        fl_line(xFunc_(p1.X), yFunc_(p1.Y),
            xFunc_(p2.X), yFunc_(p2.Y));
    }
#endif
}

void GraphWidget::draw_legend() {
    using namespace PlotParams;

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    void* font = GLUT_BITMAP_HELVETICA_12;
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    auto font = reinterpret_cast<void*>(12);
#endif

    PrintText(font,
        xFunc_(XMin), yFunc_(YMin - pixelY_ * (Margin / 2 + TickSize)),
        0, "%.1f", XMin);

    PrintText(font,
        xFunc_(XMax), yFunc_(YMin - pixelY_ * (Margin / 2 + TickSize)),
        0, "%.1f", XMax);

    PrintText(font,
        xFunc_(XMin - pixelX_ * (Margin / 2 + TickSize)), yFunc_(YMax - pixelY_ * (Margin / 2)),
        0, "%.1f", YMax);

    PrintText(font,
        xFunc_(XMin - pixelX_ * (Margin / 2 + TickSize)), yFunc_(YMin + pixelY_ * (Margin / 2)),
        0, "%.1f", YMin);
}

void GraphWidget::update_size() {
    using namespace PlotParams;

    // Update pixel scales
    pixelX_ = (XMax - XMin) / (this->w() - Margin * 2 - TickSize);
    pixelY_ = (YMax - YMin) / (this->h() - Margin * 2 - TickSize);

    /* Y Ticks */
    ticksY_.resize((TickCount + 1) * 2);
    float dTickY = (YMax - YMin) / static_cast<float>(TickCount);
    for (size_t i = 0; i <= TickCount; i++) {
        float y = YMin + static_cast<float>(i) * dTickY;
        float tickScale = (i % 2) ? 0.5f : 1.0f;
        ticksY_[i * 2] = HMM_Vec2(XMin, y);
        ticksY_[i * 2 + 1] = HMM_Vec2(XMin - static_cast<float>(TickSize) * pixelX_ * tickScale, y);
    }

    /* X Ticks */
    ticksX_.resize((TickCount + 1) * 2);
    float dTickX = (XMax - XMin) / static_cast<float>(TickCount);
    for (size_t i = 0; i <= TickCount; i++) {
        float x = XMin + static_cast<float>(i) * dTickX;
        float tickScale = (i % 2) ? 0.5f : 1.0f;
        ticksX_[i * 2] = HMM_Vec2(x, YMin);
        ticksX_[i * 2 + 1] = HMM_Vec2(x, YMin - static_cast<float>(TickSize) * pixelY_ * tickScale);
    }

#if DRAW_METHOD==DRAW_METHOD_FLTK
    if (initOffscreen_) {
        fl_delete_offscreen(offscreen_);
        fl_delete_offscreen(contourOfs_);
    }

    offscreen_ = fl_create_offscreen(this->w(), this->h());
    contourOfs_ = fl_create_offscreen(this->w(), this->h());

    initOffscreen_ = true;

    contourOfsNeedsRedraw_ = true;
#endif
}

int GraphWidget::handle(int e) {
    if (e==FL_PUSH) {
        // Set start point for search engines
        if (Fl::event_button()==FL_LEFT_MOUSE) {
            using namespace PlotParams;

            int x = Fl::event_x();
            int y = Fl::event_y();
            x -= Margin + TickSize;
            y -= Margin;

            // std::cout<<"x: "<<x<<"    y: "<<y<<std::endl;
            if (x>=0 && x<=(this->w() - Margin * 2 - TickSize) &&
                y>=0 && y<=(this->h() - Margin * 2 - TickSize)) {

                float sx = XMin + static_cast<float>(x) * pixelX_;
                float sy = YMax - static_cast<float>(y) * pixelY_;
                if (engine_) {
                    engine_->set_start_point(sx, sy);
                }
                this->redraw();
#if DRAW_METHOD==DRAW_METHOD_OPENGL
                this->invalidate();
#endif
            }
        }
        return 1;
    }

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    return Fl_Gl_Window::handle(e);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    return Fl_Widget::handle(e);
#endif
}

void GraphWidget::create_surface() {
    using namespace PlotParams;

    int rows = YDivCount + 1, cols = XDivCount + 1;
    float zmin = 0.0f, zmax = 0.0f;
    float dx = (XMax - XMin) / static_cast<float>(cols);
    float dy = (YMax - YMin) / static_cast<float>(rows);
    std::vector<float> points(rows * cols);

    for (int j=0; j<rows; j++) {
        float y = YMin + static_cast<float>(j) * dy;
        for (int i=0; i<cols; i++) {
            float x = XMin + static_cast<float>(i) * dx;
            float z = func(x, y);
            points[j * cols + i] = z;
            if (zmin>z) zmin = z;
            if (zmax<z) zmax = z;
        }
    }

    float dz = (sqrt(zmax) - sqrt(zmin)) / static_cast<float>(Palette.size());

    for (size_t i=0; i < Palette.size(); i++) {
        float z = sqrt(zmin) + static_cast<float>(i) * dz;
        contourLines_[i].init(points, cols, rows,
            XMin, YMin, XMax, YMax, z*z);
        contourFills_[i].init(points, cols, rows,
            XMin, YMin, XMax, YMax, z*z);
    }
}
