#include "pch.h"
#include "MathUtils.h"
#include "PlotWidget.h"

PlotWidget::PlotWidget(int X, int Y, int W, int H, const char* lbl)
    : Fl_Gl_Window(X, Y, W, H, lbl) {
    tick_color[0] = 0.0; tick_color[1] = 0.0; tick_color[2] = 0.0;
    axis_color[0] = 0.75; axis_color[1] = 0.75; axis_color[2] = 0.75;
    plot_color[0] = 1.0; plot_color[1] = 0.0; plot_color[2] = 0.0;
    text_color[0] = 0.25; text_color[1] = 0.25; text_color[2] = 0.25;

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

const int ALIGN_TOP = 0x01;
const int ALIGN_MIDDLE = 0x02;
const int ALIGN_BOTTOM = 0x04;
const int ALIGN_LEFT = 0x10;
const int ALIGN_CENTER = 0x20;
const int ALIGN_RIGHT = 0x40;
void PlotWidget::print_text(void* font, float x, float y, unsigned char align, const char* fmt, ...) {
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

    if (align == 0) align = ALIGN_RIGHT | ALIGN_TOP;

    if (align & ALIGN_LEFT) tx -= tw;
    else if (align & ALIGN_CENTER) tx -= tw / 2;

    if (align & ALIGN_BOTTOM) ty -= th;
    else if (align & ALIGN_MIDDLE) ty -= th / 2;

    glRasterPos2d(tx, ty);
    glutBitmapString(font, (const unsigned char*)text);
}

void PlotWidget::draw_box() {
    glColor3fv(tick_color);

    glBegin(GL_LINE_LOOP);
    glVertex2f(xmin_, ymin_);
    glVertex2f(xmax_, ymin_);
    glVertex2f(xmax_, ymax_);
    glVertex2f(xmin_, ymax_);
    glEnd();
}

void PlotWidget::draw_ticks() {
    glColor3fv(tick_color);

    glBegin(GL_LINES);
    if (ticks_x.size() > 0) {
        for (int i = 0; i <= tick_count_; i++) {
            glVertex2f(ticks_x[i * 2].x, ticks_x[i * 2].y);
            glVertex2f(ticks_x[i * 2 + 1].x, ticks_x[i * 2 + 1].y);
        }
    }

    if (ticks_x.size() > 0) {
        for (int i = 0; i <= tick_count_; i++) {
            glVertex2f(ticks_y[i * 2].x, ticks_y[i * 2].y);
            glVertex2f(ticks_y[i * 2 + 1].x, ticks_y[i * 2 + 1].y);
        }
    }
    glEnd();
}

void PlotWidget::draw_axis() {
    glColor3fv(axis_color);

    // X axis
    if (betweenf(ymin_, xaxis_, ymax_)) {
        glBegin(GL_LINES);
        glVertex2f(xmin_, xaxis_);
        glVertex2f(xmax_, xaxis_);
        glEnd();
    }

    // Y axis
    if (betweenf(xmin_, yaxis_, xmax_)) {
        glBegin(GL_LINES);
        glVertex2f(yaxis_, xmin_);
        glVertex2f(yaxis_, xmax_);
        glEnd();
    }
}

void PlotWidget::draw_heatmap() {
    static const size_t PaletteSize = 11;
    static const GLubyte Palette[PaletteSize][4] = {
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

    glBegin(GL_QUADS);
    if (points.size() > 0) {
        for (int i = 0; i < point_count_; i++) {
            double y = points[i * 2].y;
            double t = 1. - (y - ymin_) / (ymax_ - ymin_);
            size_t k = static_cast<size_t>((static_cast<double>(PaletteSize) * t)) % PaletteSize;
            glColor4ubv(Palette[k]);

            glVertex2f(points[i * 2].x, ymin_);
            glVertex2f(points[i * 2 + 1].x, ymin_);
            glVertex2f(points[i * 2 + 1].x, ymax_);
            glVertex2f(points[i * 2].x, ymax_);
        }
    }
    glEnd();

}

void PlotWidget::draw_plot() {
    glColor3fv(plot_color);
    glBegin(GL_LINES);
    if (points.size() > 0) {
        for (int i = 0; i < point_count_; i++) {
            glVertex2f(points[i * 2].x, points[i * 2].y);
            glVertex2f(points[i * 2 + 1].x, points[i * 2 + 1].y);
        }
    }
    glEnd();
}

#define LEGEND_FONT GLUT_BITMAP_HELVETICA_10
void PlotWidget::draw_legend() {
    glColor3fv(text_color);

    print_text(LEGEND_FONT, (xmax_ - xmin_) / 2.0, ymax_ + margin_ * pixel_y / 2.0,
        ALIGN_CENTER | ALIGN_MIDDLE, this->label());

    print_text(LEGEND_FONT, -tick_size_ * pixel_x, 0.0,
        ALIGN_LEFT | ALIGN_MIDDLE, "%.1f", 0.0);

    print_text(LEGEND_FONT, xmin_ - tick_size_ * pixel_x, ymin_,
        ALIGN_LEFT | ALIGN_MIDDLE, "%.1f", ymin_);

    print_text(LEGEND_FONT, xmin_ - tick_size_ * pixel_x, ymax_,
        ALIGN_LEFT | ALIGN_MIDDLE, "%.1f", ymax_);

    print_text(LEGEND_FONT, xmin_, ymin_ - tick_size_ * pixel_y,
        ALIGN_CENTER | ALIGN_BOTTOM, "%.1f", xmin_);
    print_text(LEGEND_FONT, xmax_, ymin_ - tick_size_ * pixel_y,
        ALIGN_CENTER | ALIGN_BOTTOM, "%.1f", xmax_);
}

void PlotWidget::draw() {
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

    if (!this->context_valid()) {
        glShadeModel(GL_SMOOTH);
        glClearColor(1.0, 1.0, 1.0, 0.5);
        glClearDepth(1.0);
        glDisable(GL_DEPTH_TEST);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    draw_heatmap();

    // Draw ticks and bounding box
    this->draw_box();
    this->draw_ticks();

    this->draw_axis();

    this->draw_legend();

    this->draw_plot();

    glFinish();
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
