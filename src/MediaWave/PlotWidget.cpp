#include "pch.h"
#include "MathUtils.h"
#include "PlotWidget.h"

static const std::vector<std::array<uint8_t, 4>> Palette = {
    {0xa5, 0x00, 0x27, 0x00},
    {0xd7, 0x30, 0x27, 0x00},
    {0xf4, 0x6d, 0x43, 0x00},
    {0xfd, 0xae, 0x61, 0x00},
    {0xfe, 0xe0, 0x90, 0x00},
    {0xff, 0xff, 0xbf, 0x00},
    {0xe0, 0xf3, 0xf9, 0x00},
    {0xab, 0xd9, 0xea, 0x00},
    {0x74, 0xad, 0xd1, 0x00},
    {0x45, 0x75, 0xb4, 0x00},
    {0x31, 0x36, 0x95, 0x00},
};


PlotWidget::PlotWidget(int X, int Y, int W, int H, const char* lbl)
#ifdef DRAW_OPENGL
    : Fl_Gl_Window(X, Y, W, H, lbl) {
#else
    : Fl_Widget(X, Y, W, H, lbl) {
#endif
    tick_color = { 0.0f, 0.0f, 0.0f, 1.0f };
    axis_color = { 0.75f, 0.75f, 0.75f, 1.0f };
    plot_color = { 255, 0, 0, 255 };
    text_color = { 0.25f, 0.25f, 0.25f, 1.0f };

    this->axis(DefXAxis, DefYAxis);
    this->view_range(DefXMin, DefXMax, DefYMin, DefYMax);
    this->margin(DefMargin);
    this->ticks(DefTickCount, DefTickSize);
}

void PlotWidget::pixel_scale() {
    pixel_x = (xmax_ - xmin_) / (this->w() - margin_ * 2 - tick_size_);
    pixel_y = (ymax_ - ymin_) / (this->h() - margin_ * 2 - tick_size_);
}

void PlotWidget::axis(float xaxis, float yaxis) {
    this->xaxis_ = xaxis;
    this->yaxis_ = yaxis;
}

void PlotWidget::view_range(float xmin, float xmax, float ymin, float ymax) {
    this->xmin_ = xmin;
    this->xmax_ = xmax;
    this->ymin_ = ymin;
    this->ymax_ = ymax;
}

void PlotWidget::ticks(int count, int size) {
    this->tick_count_ = count;
    this->tick_size_ = size;

    ticks_x.resize((tick_count_ + 1) * 2);
    ticks_y.resize((tick_count_ + 1) * 2);

    pixel_scale();

    // Calculate x&y tick positions
    float dtick;
    float x, y, tick_scale;

    // Y ticks
    dtick = (ymax_ - ymin_) / tick_count_;
    for (int i = 0; i <= tick_count_; i++) {
        y = ymin_ + i * dtick;

        ticks_y[i * 2].x = xmin_;
        ticks_y[i * 2].y = y;

        tick_scale = (i % 2) ? 0.5 : 1.0;
        ticks_y[i * 2 + 1].x = xmin_ - tick_size_ * pixel_x * tick_scale;
        ticks_y[i * 2 + 1].y = y;
    }

    // X ticks
    dtick = (xmax_ - xmin_) / tick_count_;
    for (int i = 0; i <= tick_count_; i++) {
        x = xmin_ + i * dtick;

        ticks_x[i * 2].x = x;
        ticks_x[i * 2].y = ymin_;

        tick_scale = (i % 2) ? 0.5 : 1.0; // 1.0
        ticks_x[i * 2 + 1].x = x;
        ticks_x[i * 2 + 1].y = ymin_ - tick_size_ * pixel_y * tick_scale;
    }
}

void PlotWidget::margin(int m) {
    this->margin_ = m;
}

#ifdef DRAW_OPENGL
namespace Align {
    const uint8_t None = 0x00;

    const uint8_t Top = 0x01;
    const uint8_t Middle = 0x02;
    const uint8_t Bottom = 0x04;

    const uint8_t Left = 0x10;
    const uint8_t Center = 0x20;
    const uint8_t Right = 0x40;

    const uint8_t Default = Align::Right | Align::Top;
}

void PlotWidget::print_text(void* font, float x, float y, uint8_t align, const char* fmt, ...) {
    char text[256];
    va_list ap;

    if (fmt == NULL) return;

    va_start(ap, fmt);
    vsnprintf(text, 256, fmt, ap);
    va_end(ap);

    int w, h;
    h = glutBitmapHeight(font);

    w = 0;
    char* p = (char*)text;
    while (*p != '\0') {
        w += glutBitmapWidth(font, *p);
        p++;
    }

    float tx, ty, tw, th;
    tx = x;
    ty = y;
    tw = w * pixel_x;
    th = h * pixel_y;

    if (align == Align::None) align = Align::Default;

    if (align & Align::Left) tx -= tw;
    else if (align & Align::Center) tx -= tw / 2;

    if (align & Align::Bottom) ty -= th;
    else if (align & Align::Middle) ty -= th / 2;

    glRasterPos2d(tx, ty);
    glutBitmapString(font, (const unsigned char*)text);
}
#endif

void PlotWidget::draw_box() {
#ifdef DRAW_OPENGL
    glColor4fv(tick_color.data());

    glBegin(GL_LINE_LOOP);
    glVertex2f(xmin_, ymin_);
    glVertex2f(xmax_, ymin_);
    glVertex2f(xmax_, ymax_);
    glVertex2f(xmin_, ymax_);
    glEnd();
#else
    fl_color(fl_rgb_color(tick_color[0], tick_color[1], tick_color[2]));

    fl_loop(
        get_x(xmin_), get_y(ymin_),
        get_x(xmax_), get_y(ymin_),
        get_x(xmax_), get_y(ymax_),
        get_x(xmin_), get_y(ymax_));
#endif
}

void PlotWidget::draw_ticks() {
#ifdef DRAW_OPENGL
    glColor4fv(tick_color.data());
    glBegin(GL_LINES);
#else
    fl_color(fl_rgb_color(tick_color[0], tick_color[1], tick_color[2]));
#endif

    if (ticks_x.size() > 0) {
        for (int i = 0; i <= tick_count_; i++) {
#ifdef DRAW_OPENGL
            glVertex2f(ticks_x[i * 2].x, ticks_x[i * 2].y);
            glVertex2f(ticks_x[i * 2 + 1].x, ticks_x[i * 2 + 1].y);
#else
            fl_line(get_x(ticks_x[i * 2].x), get_y(ticks_x[i * 2].y),
                get_x(ticks_x[i * 2 + 1].x), get_y(ticks_x[i * 2 + 1].y));
#endif
        }
    }

    if (ticks_x.size() > 0) {
        for (int i = 0; i <= tick_count_; i++) {
#ifdef DRAW_OPENGL
            glVertex2f(ticks_y[i * 2].x, ticks_y[i * 2].y);
            glVertex2f(ticks_y[i * 2 + 1].x, ticks_y[i * 2 + 1].y);
#else
            fl_line(get_x(ticks_y[i * 2].x), get_y(ticks_y[i * 2].y),
                get_x(ticks_y[i * 2+1].x), get_y(ticks_y[i * 2+1].y));
#endif
        }
    }

#ifdef DRAW_OPENGL
    glEnd();
#endif
}

void PlotWidget::draw_axis() {
#ifdef DRAW_OPENGL
    glColor4fv(axis_color.data());
#else
    fl_color(fl_rgb_color(axis_color[0], axis_color[1], axis_color[2]));
#endif

    // X axis
    if (betweenf(ymin_, xaxis_, ymax_)) {
#ifdef DRAW_OPENGL
        glBegin(GL_LINES);
        glVertex2f(xmin_, xaxis_);
        glVertex2f(xmax_, xaxis_);
        glEnd();
#else
        fl_line(get_x(xmin_), get_y(xaxis_),
            get_x(xmax_), get_y(xaxis_));
#endif
    }

    // Y axis
    if (betweenf(ymin_, yaxis_, ymax_)) {
#ifdef DRAW_OPENGL
        glBegin(GL_LINES);
        glVertex2f(xmin_, yaxis_);
        glVertex2f(xmax_, yaxis_);
        glEnd();
#else
        fl_line(get_x(xmin_), get_y(yaxis_),
            get_x(xmax_), get_y(yaxis_));
#endif
    }
}

void PlotWidget::draw_heatmap() {
#ifdef DRAW_OPENGL
    glBegin(GL_QUADS);
#endif
    if (points.size() > 0) {
        for (int i = 0; i < point_count_; i++) {
            double y = points[i * 2].y;
            double t = 1. - (y - ymin_) / (ymax_ - ymin_);
            size_t k = static_cast<size_t>((static_cast<double>(Palette.size()) * t)) % Palette.size();

#ifdef DRAW_OPENGL
            glColor4ubv(Palette[k].data());

            glVertex2f(points[i * 2].x, ymin_);
            glVertex2f(points[i * 2 + 1].x, ymin_);
            glVertex2f(points[i * 2 + 1].x, ymax_);
            glVertex2f(points[i * 2].x, ymax_);
#else
            fl_color(fl_rgb_color(Palette[k][0], Palette[k][1], Palette[k][2]));

            fl_polygon(
                get_x(points[i * 2].x), get_y(ymin_),
                get_x(points[i * 2 + 1].x), get_y(ymin_),
                get_x(points[i * 2 + 1].x), get_y(ymax_),
                get_x(points[i * 2].x), get_y(ymax_)
            );
#endif
        }
    }

#ifdef DRAW_OPENGL
    glEnd();
#endif
}

void PlotWidget::draw_plot() {
#ifdef DRAW_OPENGL
    glColor4ubv(plot_color.data());
    glBegin(GL_LINES);
#else
    fl_color(fl_rgb_color(plot_color[0], plot_color[1], plot_color[2]));
#endif

    if (points.size() > 0) {
        for (int i = 0; i < point_count_; i++) {
#ifdef DRAW_OPENGL
            glVertex2f(points[i * 2].x, points[i * 2].y);
            glVertex2f(points[i * 2 + 1].x, points[i * 2 + 1].y);
#else
            fl_line(get_x(points[i * 2].x), get_y(points[i * 2].y),
                get_x(points[i * 2+1].x), get_y(points[i * 2+1].y));
#endif
        }
    }

#ifdef DRAW_OPENGL
    glEnd();
#endif
}

#define LEGEND_FONT GLUT_BITMAP_HELVETICA_10
void PlotWidget::draw_legend() {
#ifdef DRAW_OPENGL
    glColor4fv(text_color.data());

    print_text(LEGEND_FONT, (xmax_ - xmin_) / 2.0, ymax_ + margin_ * pixel_y / 2.0,
        Align::Center | Align::Middle, this->label());

    print_text(LEGEND_FONT, -tick_size_ * pixel_x, 0.0,
        Align::Left | Align::Middle, "%.1f", 0.0);

    print_text(LEGEND_FONT, xmin_ - tick_size_ * pixel_x, ymin_,
        Align::Left | Align::Middle, "%.1f", ymin_);

    print_text(LEGEND_FONT, xmin_ - tick_size_ * pixel_x, ymax_,
        Align::Left | Align::Middle, "%.1f", ymax_);

    print_text(LEGEND_FONT, xmin_, ymin_ - tick_size_ * pixel_y,
        Align::Center | Align::Bottom, "%.1f", xmin_);
    print_text(LEGEND_FONT, xmax_, ymin_ - tick_size_ * pixel_y,
        Align::Center | Align::Bottom, "%.1f", xmax_);
#endif
}

void PlotWidget::draw() {
#ifdef DRAW_OPENGL
    if (!this->valid()) {
        pixel_scale();
        glViewport(0, 0, this->w(), this->h());
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(xmin_ - pixel_x * (margin_ + tick_size_),
            xmax_ + pixel_x * margin_,
            ymin_ - pixel_y * (margin_ + tick_size_),
            ymax_ + pixel_y * margin_);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
#endif

#ifdef DRAW_OPENGL
    if (!this->context_valid()) {
        glShadeModel(GL_SMOOTH);
        glClearColor(1.0, 1.0, 1.0, 0.5);
        glClearDepth(1.0);
        glDisable(GL_DEPTH_TEST);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }
#endif

#ifdef DRAW_OPENGL
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
#else
    if (damage() & FL_DAMAGE_ALL) {
        fl_draw_box(FL_FLAT_BOX,
            this->x(), this->y(),
            this->w(), this->h(),
            fl_rgb_color(255));
    }
#endif

#ifndef DRAW_OPENGL
    if (damage() & (FL_DAMAGE_ALL | 1)) {
#endif

    draw_heatmap();

    // Draw ticks and bounding box
    draw_box();
    draw_ticks();

    draw_axis();

    draw_legend();

    draw_plot();

#ifndef DRAW_OPENGL
    }
#endif

#ifdef DRAW_OPENGL
    glFinish();
#endif
}

void PlotWidget::plot(int count, const std::vector<float>& x, const std::vector<float>& y) {
    this->point_count_ = count;

    points.resize((this->point_count_ + 1) * 2);

    for (int i = 0; i < this->point_count_; i++) {
        points[i * 2].x = x[i];
        points[i * 2].y = y[i];

        points[i * 2 + 1].x = x[i + 1];
        points[i * 2 + 1].y = y[i];
    }
}
