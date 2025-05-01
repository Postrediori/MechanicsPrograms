#include "pch.h"
#include "MathUtils.h"
#include "PlotWidget.h"

static const std::vector<Fl_Color> Palette = {
    fl_rgb_color(0xa5, 0x00, 0x27),
    fl_rgb_color(0xd7, 0x30, 0x27),
    fl_rgb_color(0xf4, 0x6d, 0x43),
    fl_rgb_color(0xfd, 0xae, 0x61),
    fl_rgb_color(0xfe, 0xe0, 0x90),
    fl_rgb_color(0xff, 0xff, 0xbf),
    fl_rgb_color(0xe0, 0xf3, 0xf9),
    fl_rgb_color(0xab, 0xd9, 0xea),
    fl_rgb_color(0x74, 0xad, 0xd1),
    fl_rgb_color(0x45, 0x75, 0xb4),
    fl_rgb_color(0x31, 0x36, 0x95),
};

#if DRAW_METHOD==DRAW_METHOD_OPENGL
#define SET_FL_COLOR_TO_GL(c) { \
    GLubyte r, g, b, a; \
    Fl::get_color((c), r, g, b, a); \
    glColor4ub(r, g, b, a); \
    }
#endif

PlotWidget::PlotWidget(int X, int Y, int W, int H, const char* lbl)
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    : Fl_Gl_Window(X, Y, W, H, lbl) {
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    : Fl_Widget(X, Y, W, H, lbl) {
#endif
    tick_color = fl_rgb_color(0, 0, 0);
    axis_color = fl_rgb_color(192, 192, 192);
    plot_color = fl_rgb_color(255, 0, 0);
    text_color = fl_rgb_color(64, 64, 64);

    this->axis(PlotDefaults::XAxis, PlotDefaults::YAxis);
    this->view_range(PlotDefaults::XMin, PlotDefaults::XMax, PlotDefaults::YMin, PlotDefaults::YMax);
    this->margin(PlotDefaults::Margin);
    this->ticks(PlotDefaults::TickCount, PlotDefaults::TickSize);
}

PlotWidget::~PlotWidget() {
#if DRAW_METHOD==DRAW_METHOD_FLTK
    if (initOffscreen_) {
        initOffscreen_ = false;
        fl_delete_offscreen(offscreen_);
    }
#endif
}

void PlotWidget::resize(int x, int y, int w, int h) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    Fl_Gl_Window::resize(x, y, w, h);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    Fl_Widget::resize(x, y, w, h);

    this->ticks(PlotDefaults::TickCount, PlotDefaults::TickSize);

    if (initOffscreen_) {
        fl_delete_offscreen(offscreen_);
        offscreen_ = fl_create_offscreen(this->w(), this->h());
    }
#endif
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

void PlotWidget::print_text(float x, float y, uint8_t align, const char* fmt, ...) {
    const size_t BufferLen = 256;
    char text[BufferLen] = { 0 };
    va_list ap;

    if (!fmt) {
        return;
    }

    va_start(ap, fmt);
    vsnprintf(text, BufferLen-1, fmt, ap);
    va_end(ap);

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    void* font = GLUT_BITMAP_HELVETICA_12;

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

    if (align == TextAlign::Unspecified) align = TextAlign::Default;

    if (align & TextAlign::Left) tx -= tw;
    else if (align & TextAlign::Center) tx -= tw / 2;

    if (align & TextAlign::Bottom) ty -= th;
    else if (align & TextAlign::Middle) ty -= th / 2;

    glRasterPos2d(tx, ty);
    glutBitmapString(font, (const unsigned char*)text);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_font(FL_HELVETICA, 14);

    int w{ 0 }, h{ 0 };
    fl_measure(text, w, h);

    float x0{ get_x(x) }, y0{ get_y(y) };

    if (align & TextAlign::Left) {
        x0 -= w;
    }
    else if (align & TextAlign::Center) {
        x0 -= w / 2;
    }

    if (align & TextAlign::Bottom) {
        y0 += h;
    }
    else if (align & TextAlign::Middle) {
        y0 += h / 2;
    }

    fl_draw(text, x0, y0);
#endif
}

void PlotWidget::draw_box() {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    SET_FL_COLOR_TO_GL(tick_color);

    glBegin(GL_LINE_LOOP);
    glVertex2f(xmin_, ymin_);
    glVertex2f(xmax_, ymin_);
    glVertex2f(xmax_, ymax_);
    glVertex2f(xmin_, ymax_);
    glEnd();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(tick_color);

    fl_loop(
        get_x(xmin_), get_y(ymin_),
        get_x(xmax_), get_y(ymin_),
        get_x(xmax_), get_y(ymax_),
        get_x(xmin_), get_y(ymax_));
#endif
}

void PlotWidget::draw_ticks() {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    SET_FL_COLOR_TO_GL(tick_color);
    glBegin(GL_LINES);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(tick_color);
#endif

    if (ticks_x.size() > 0) {
        for (int i = 0; i <= tick_count_; i++) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
            glVertex2f(ticks_x[i * 2].x, ticks_x[i * 2].y);
            glVertex2f(ticks_x[i * 2 + 1].x, ticks_x[i * 2 + 1].y);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
            fl_line(get_x(ticks_x[i * 2].x), get_y(ticks_x[i * 2].y),
                get_x(ticks_x[i * 2 + 1].x), get_y(ticks_x[i * 2 + 1].y));
#endif
        }
    }

    if (ticks_x.size() > 0) {
        for (int i = 0; i <= tick_count_; i++) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
            glVertex2f(ticks_y[i * 2].x, ticks_y[i * 2].y);
            glVertex2f(ticks_y[i * 2 + 1].x, ticks_y[i * 2 + 1].y);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
            fl_line(get_x(ticks_y[i * 2].x), get_y(ticks_y[i * 2].y),
                get_x(ticks_y[i * 2+1].x), get_y(ticks_y[i * 2+1].y));
#endif
        }
    }

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glEnd();
#endif
}

void PlotWidget::draw_axis() {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    SET_FL_COLOR_TO_GL(axis_color);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(axis_color);
#endif

    // X axis
    if (betweenf(ymin_, xaxis_, ymax_)) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
        glBegin(GL_LINES);
        glVertex2f(xmin_, xaxis_);
        glVertex2f(xmax_, xaxis_);
        glEnd();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
        fl_line(get_x(xmin_), get_y(xaxis_),
            get_x(xmax_), get_y(xaxis_));
#endif
    }

    // Y axis
    if (betweenf(ymin_, yaxis_, ymax_)) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
        glBegin(GL_LINES);
        glVertex2f(xmin_, yaxis_);
        glVertex2f(xmax_, yaxis_);
        glEnd();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
        fl_line(get_x(xmin_), get_y(yaxis_),
            get_x(xmax_), get_y(yaxis_));
#endif
    }
}

void PlotWidget::draw_heatmap() {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glBegin(GL_QUADS);
#endif
    if (points.size() > 0) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
        for (int i = 0; i < point_count_; i++) {
            double y = points[i * 2].y;
            double t = 1. - (y - ymin_) / (ymax_ - ymin_);
            size_t k = static_cast<size_t>((static_cast<double>(Palette.size() - 1) * t));

            SET_FL_COLOR_TO_GL(Palette[k]);

            glVertex2f(points[i * 2].x, bar_plot_ ? 0 : ymin_);
            glVertex2f(points[i * 2 + 1].x, bar_plot_ ? 0 : ymin_);
            glVertex2f(points[i * 2 + 1].x, bar_plot_ ? y : ymax_);
            glVertex2f(points[i * 2].x, bar_plot_ ? y : ymax_);
        }
#elif DRAW_METHOD==DRAW_METHOD_FLTK
        if (bar_plot_) {
            for (int i = 0; i < point_count_; i++) {
                double y = points[i * 2].y;
                double t = 1. - (y - ymin_) / (ymax_ - ymin_);
                size_t k = static_cast<size_t>((static_cast<double>(Palette.size() - 1) * t));

                fl_color(Palette[k]);

                fl_polygon(
                    get_x(points[i * 2].x), get_y(bar_plot_ ? 0 : ymin_),
                    get_x(points[i * 2 + 1].x), get_y(bar_plot_ ? 0 : ymin_),
                    get_x(points[i * 2 + 1].x), get_y(bar_plot_ ? y : ymax_),
                    get_x(points[i * 2].x), get_y(bar_plot_ ? y : ymax_)
                );
            }
        }
        else {
            size_t kPrev{ 0 };
            int j{ 0 };

            for (int i = 0; i <= point_count_; i++) {
                double y = points[i * 2].y;
                double t = 1. - (y - ymin_) / (ymax_ - ymin_);
                size_t k = static_cast<size_t>((static_cast<double>(Palette.size()) * t)) % Palette.size();
                if (i == 0) {
                    kPrev = k;
                    continue;
                }

                if (k != kPrev || (i == point_count_)) {
                    // Level changed, draw new part of heatmap

                    fl_color(Palette[kPrev]);
                    fl_polygon(
                        get_x(points[j * 2].x), get_y(ymin_),
                        get_x(points[(i - 1) * 2 + 1].x), get_y(ymin_),
                        get_x(points[(i - 1) * 2 + 1].x), get_y(ymax_),
                        get_x(points[j * 2].x), get_y(ymax_)
                    );

                    // Mark start of the next heatmap part
                    j = i;
                }

                kPrev = k;
            }
        }
#endif
    }

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glEnd();
#endif
}

void PlotWidget::draw_plot() {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    SET_FL_COLOR_TO_GL(plot_color);
    glBegin(GL_LINES);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(plot_color);
#endif

    if (points.size() > 0) {
        for (int i = 0; i < point_count_; i++) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
            glVertex2f(points[i * 2].x, points[i * 2].y);
            glVertex2f(points[i * 2 + 1].x, points[i * 2 + 1].y);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
            fl_line(get_x(points[i * 2].x), get_y(points[i * 2].y),
                get_x(points[i * 2 + 1].x), get_y(points[i * 2 + 1].y));
#endif
        }
    }

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glEnd();
#endif
}

void PlotWidget::draw_legend() {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    SET_FL_COLOR_TO_GL(text_color);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(text_color);
#endif

    print_text((xmax_ - xmin_) / 2.0, ymax_ + margin_ * pixel_y / 2.0,
        TextAlign::Center | TextAlign::Middle, this->label());

    print_text(-tick_size_ * pixel_x, 0.0,
        TextAlign::Left | TextAlign::Middle, "%.1f", 0.0);

    print_text(xmin_ - tick_size_ * pixel_x, ymin_,
        TextAlign::Left | TextAlign::Middle, "%.1f", ymin_);

    print_text(xmin_ - tick_size_ * pixel_x, ymax_,
        TextAlign::Left | TextAlign::Middle, "%.1f", ymax_);

    print_text(xmin_, ymin_ - tick_size_ * pixel_y,
        TextAlign::Center | TextAlign::Bottom, "%.1f", xmin_);
    print_text(xmax_, ymin_ - tick_size_ * pixel_y,
        TextAlign::Center | TextAlign::Bottom, "%.1f", xmax_);
}

void PlotWidget::draw() {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    if (!this->valid()) {
        this->ticks(PlotDefaults::TickCount, PlotDefaults::TickSize);

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

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    if (!this->context_valid()) {
        glShadeModel(GL_SMOOTH);
        glClearColor(1.0, 1.0, 1.0, 0.5);
        glClearDepth(1.0);
        glDisable(GL_DEPTH_TEST);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    if (!initOffscreen_) {
        initOffscreen_ = true;
        offscreen_ = fl_create_offscreen(this->w(), this->h());
    }
#endif

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_begin_offscreen(offscreen_);

    fl_color(fl_rgb_color(255));
    fl_rectf(0, 0, this->w(), this->h());
#endif

    draw_heatmap();

    // Draw ticks and bounding box
    draw_box();
    draw_ticks();

    draw_axis();

    draw_legend();

    draw_plot();

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glFinish();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_end_offscreen();

    fl_copy_offscreen(this->x(), this->y(), this->w(), this->h(),
        offscreen_, 0, 0);
#endif
}

float PlotWidget::get_x(float x) const {
    return (x - xmin_) / pixel_x + margin_ + tick_size_;
}

float PlotWidget::get_y(float y) const {
    return (ymax_ - y) / pixel_y + margin_;
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
