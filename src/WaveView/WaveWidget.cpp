#include "pch.h"
#include "SurfaceFunction.h"
#include "WaveModel.h"
#include "WaveWidget.h"

using ByteColor = std::array<uint8_t, 4>;

const ByteColor BoxColor = { 192, 192, 192, 255 };
const ByteColor AxesColor = { 128, 128, 128, 255 };
const ByteColor TicksColor = { 64, 64, 64, 255 };
const ByteColor ModelLinesColor = { 0, 0, 0, 255 };
const ByteColor ModelPointsColor = { 0, 0, 0, 255 }; // {128, 192, 255, 255};
const ByteColor TextColor = { 0, 0, 0, 255 };

struct Gradient {
    ByteColor colorMin;
    ByteColor colorMax;

    ByteColor GetColor(double x) const {
        ByteColor color = { 255 };
        for (size_t i = 0; i < 3; i++) {
            color[i] = static_cast<uint8_t>(static_cast<double>(colorMin[i]) +
                static_cast<double>(colorMax[i] - colorMin[i]) * x);
        }
        return color;
    }
};

// Blue-white gradient
// f7fbff - 08306b
static const Gradient HeatmapGradient = {
    { 0xf7, 0xfb, 0xff, 0xff },
    { 0x08, 0x30, 0x6b, 0xff }
};

WaveWidget::WaveWidget(int X, int Y, int W, int H, WaveModel* model, const char* l)
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    : Fl_Gl_Window(X, Y, W, H, l),
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    : Fl_Widget(X, Y, W, H, l),
#endif
    model_(model) {
    using namespace PlotDefaults;

    pixelX_ = (XMax - XMin) / (this->w() - Margin * 2 - TickSize);
    pixelY_ = (YMax - YMin) / (this->h() - Margin * 2 - TickSize);

    // Y Ticks
    ticksY.resize((TickCount + 1) * 2);
    double dTick = (YMax - YMin) / TickCount;
    for (int i = 0; i <= TickCount; i++) {
        double y = YMin + i * dTick;
        ticksY[i * 2].x = XMin;
        ticksY[i * 2].y = y;

        double tickScale = (i % 2) ? 0.5 : 1.0;
        ticksY[i * 2 + 1].x = XMin - TickSize * pixelX_ * tickScale;
        ticksY[i * 2 + 1].y = y;
    }

    // X Ticks
    ticksX.resize((TickCount + 1) * 2);
    dTick = (XMax - XMin) / TickCount;
    for (int i = 0; i <= TickCount; i++) {
        double x = XMin + i * dTick;
        ticksX[i * 2].y = YMin;
        ticksX[i * 2].x = x;

        double tickScale = (i % 2) ? 0.5 : 1.0;
        ticksX[i * 2 + 1].y = YMin - TickSize * pixelY_ * tickScale;
        ticksX[i * 2 + 1].x = x;
    }
}

WaveWidget::~WaveWidget() {
#if DRAW_METHOD==DRAW_METHOD_FLTK
    if (initOffscreen_) {
        initOffscreen_ = false;
        fl_delete_offscreen(offscreen_);
    }
#endif
}

void WaveWidget::draw() {
    using namespace PlotDefaults;

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    if (!this->valid()) {
        pixelX_ = (XMax - XMin) / (this->w() - Margin * 2 - TickSize);
        pixelY_ = (YMax - YMin) / (this->h() - Margin * 2 - TickSize);

        glViewport(0, 0, this->w(), this->h());
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(XMin - pixelX_ * (Margin + TickSize),
            XMax + pixelX_ * Margin,
            YMin - pixelY_ * (Margin + TickSize),
            YMax + pixelY_ * Margin,
            -1.0, 1.0);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_begin_offscreen(offscreen_);

    fl_color(fl_rgb_color(255));
    fl_rectf(0, 0, this->w(), this->h());
#endif

    draw_heatmap();
    draw_box();
    draw_axes();
    draw_model();
    draw_ticks();
    draw_legend();

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glFinish();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_end_offscreen();

    fl_copy_offscreen(this->x(), this->y(), this->w(), this->h(),
        offscreen_, 0, 0);
#endif
}

void WaveWidget::draw_heatmap() {
    using namespace PlotDefaults;

    if (!model_) {
        return;
    }
    ByteColor layerColor;

    double scalex = (XMax - XMin) / model_->delta;
    double scalez = (/*YMax*/ -YMin) / model_->h;

    for (int j = 0; j < model_->zn - 1; j++) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
        glBegin(GL_TRIANGLE_STRIP);
#endif

        double depth = static_cast<double>(j) / static_cast<double>(model_->zn - 1);
        layerColor = HeatmapGradient.GetColor(depth);
#if DRAW_METHOD==DRAW_METHOD_OPENGL
        glColor4ubv(layerColor.data());
#elif DRAW_METHOD==DRAW_METHOD_FLTK
        fl_color(fl_rgb_color(layerColor[0], layerColor[1], layerColor[2]));
#endif

#if DRAW_METHOD==DRAW_METHOD_OPENGL
        for (int i = 0; i < model_->xn; i++) {
            const auto& p0 = model_->points[i * model_->zn + j];
            const auto& p1 = model_->points[i * model_->zn + j + 1];
            glVertex3f(p1.x * scalex + XMin,
                ((model_->h + p1.z) * scalez) + YMin, 0.0);
            glVertex3f(p0.x * scalex + XMin,
                ((model_->h + p0.z) * scalez) + YMin, 0.0);
        }
#elif DRAW_METHOD==DRAW_METHOD_FLTK
        fl_begin_polygon();
        for (int i = 0; i < model_->xn; i++) {
            const auto& p0 = model_->points[i * model_->zn + j];
            fl_vertex(get_x(p0.x * scalex + XMin), get_y((model_->h + p0.z) * scalez + YMin));
        }
        for (int i = model_->xn-1; i >= 0; i--) {
            const auto& p0 = model_->points[i * model_->zn + j + 1];
            fl_vertex(get_x(p0.x * scalex + XMin), get_y((model_->h + p0.z) * scalez + YMin));
        }
        fl_end_polygon();
#endif

#if DRAW_METHOD==DRAW_METHOD_OPENGL
        glEnd();
#endif
    }
}

void WaveWidget::draw_box() {
    using namespace PlotDefaults;

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glColor4ubv(BoxColor.data());

    glLineWidth(2);
    glBegin(GL_LINE_STRIP);
    glVertex3f(XMax, YMax, 0.0);
    glVertex3f(XMax, YMin, 0.0);
    glVertex3f(XMin, YMin, 0.0);
    glVertex3f(XMin, YMax, 0.0);
    glEnd();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(fl_rgb_color(BoxColor[0], BoxColor[1], BoxColor[2]));

    fl_line_style(FL_SOLID, 2);
    fl_line(
        get_x(XMax), get_y(YMax),
        get_x(XMax), get_y(YMin));
    fl_line(
        get_x(XMax), get_y(YMin),
        get_x(XMin), get_y(YMin));
    fl_line(
        get_x(XMin), get_y(YMin),
        get_x(XMin), get_y(YMax));
#endif
}

void WaveWidget::draw_axes() {
    using namespace PlotDefaults;

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glColor4ubv(AxesColor.data());

    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex3f(XMin, 0.0, 0.0);
    glVertex3f(XMax, 0.0, 0.0);
    glEnd();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(fl_rgb_color(AxesColor[0], AxesColor[1], AxesColor[2]));

    fl_line_style(FL_SOLID, 1);
    fl_line(
        get_x(XMin), get_y(0.0),
        get_x(XMax), get_y(0.0)
    );
#endif
}

void WaveWidget::draw_model() {
    using namespace PlotDefaults;

    if (!model_) {
        return;
    }

    double scalex = (XMax - XMin) / model_->delta;
    double scalez = (/*YMax*/ -YMin) / model_->h;

    // Lines
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glColor4ubv(ModelLinesColor.data());
    glLineWidth(1.0);
    for (int j = 0; j < model_->zn; j++) {
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < model_->xn - 1; i++) {
            const auto& p0 = model_->points[i * model_->zn + j];
            const auto& p1 = model_->points[(i + 1) * model_->zn + j];

            glVertex3f(p0.x * scalex + XMin,
                ((model_->h + p0.z) * scalez) + YMin, 0.0);
            glVertex3f(p1.x * scalex + XMin,
                ((model_->h + p1.z) * scalez) + YMin, 0.0);
        }
        glEnd();
    }
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(fl_rgb_color(ModelLinesColor[0], ModelLinesColor[1], ModelLinesColor[2]));
    fl_line_style(FL_SOLID, 1);

    for (int j = 0; j < model_->zn; j++) {
        fl_begin_line();
        for (int i = 0; i < model_->xn; i++) {
            const auto& p0 = model_->points[i * model_->zn + j];

            fl_vertex(get_x(p0.x * scalex + XMin), get_y((model_->h + p0.z) * scalez + YMin));
        }
        fl_end_line();
    }
#endif

    // Points
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glColor4ubv(ModelPointsColor.data());
    glPointSize(3.0);

    glBegin(GL_POINTS);
    for (int i = 0; i < model_->xn; i++) {
        for (int j = 0; j < model_->zn; j++) {
            const auto& p = model_->points[i * model_->zn + j];
            glVertex3f(p.x * scalex + XMin,
                ((model_->h + p.z) * scalez) + YMin, 0.0);
        }
    }
    glEnd();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(fl_rgb_color(ModelPointsColor[0], ModelPointsColor[1], ModelPointsColor[2]));

    for (int i = 0; i < model_->xn; i++) {
        for (int j = 0; j < model_->zn; j++) {
            const auto& p = model_->points[i * model_->zn + j];
            auto x = get_x(p.x * scalex + XMin);
            auto y = get_y((model_->h + p.z) * scalez + YMin);

            constexpr double PointSize = 4.0;
            fl_rectf(x - PointSize / 2, y - PointSize / 2, PointSize, PointSize);
        }
    }
#endif
}

void WaveWidget::draw_ticks() {
    using namespace PlotDefaults;

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glColor4ubv(TicksColor.data());

    glBegin(GL_LINES);
    for (int i = 0; i <= TickCount; i++) {
        glVertex2d(ticksX[i * 2].x, ticksX[i * 2].y);
        glVertex2d(ticksX[i * 2 + 1].x, ticksX[i * 2 + 1].y);

        glVertex2d(ticksY[i * 2].x, ticksY[i * 2].y);
        glVertex2d(ticksY[i * 2 + 1].x, ticksY[i * 2 + 1].y);
    }
    glEnd();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(fl_rgb_color(TicksColor[0], TicksColor[1], TicksColor[2]));
    fl_line_style(FL_SOLID, 1);

    for (int i = 0; i <= TickCount; i++) {
        fl_line(
            get_x(ticksX[i * 2].x), get_y(ticksX[i * 2].y),
            get_x(ticksX[i * 2 + 1].x), get_y(ticksX[i * 2 + 1].y)
        );

        fl_line(
            get_x(ticksY[i * 2].x), get_y(ticksY[i * 2].y),
            get_x(ticksY[i * 2 + 1].x), get_y(ticksY[i * 2 + 1].y)
        );
    }
#endif
}

void WaveWidget::draw_legend() {
    using namespace PlotDefaults;

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    auto headerFont = GLUT_BITMAP_HELVETICA_18;
    auto textFont = GLUT_BITMAP_HELVETICA_12;
#else
    auto headerFont = reinterpret_cast<void*>(18);
    auto textFont = reinterpret_cast<void*>(12);
#endif

    // Draw widget label
    draw_text(headerFont, 0, YMax,
        TextAlign::Center | TextAlign::Top,
        "%s", this->label());

    // Draw model time
    draw_text(headerFont, XMin, YMax,
        TextAlign::Right | TextAlign::Top,
        "time : %.4f", model_->time);

    // Axes labels
    // X-Axis labels
    draw_text(textFont, XMin, YMin,
        TextAlign::Right | TextAlign::Bottom,
        "%.1f", 0.0);

    draw_text(textFont, XMax, YMin,
        TextAlign::Left | TextAlign::Bottom,
        "%.1f", model_->delta);

    // Y-Axis labels
    draw_text(textFont, XMin, YMax,
        TextAlign::Left | TextAlign::Bottom,
        "%.1f", YMax);

    // Water level
    draw_text(textFont, XMin, 0.0,
        TextAlign::Left | TextAlign::Middle,
        "%.1f", 0.0);

    draw_text(textFont, XMin, YMin,
        TextAlign::Left | TextAlign::Top,
        "%.1f", -model_->h);
}

void WaveWidget::draw_text(void* font, double x, double y, uint8_t align, const char* fmt, ...) {
    const size_t BufferLen = 256;
    char text[BufferLen] = { 0 };
    va_list ap;

    if (!fmt) {
        return;
    }

    va_start(ap, fmt);
    vsnprintf(text, BufferLen - 1, fmt, ap);
    va_end(ap);

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    int h = glutBitmapHeight(font);

    int w = 0;
    char* p = (char*)text;
    while (*p != '\0') {
        w += glutBitmapWidth((void*)(font), *p);
        p++;
    }

    float tx = x;
    float ty = y;
    float tw = w * pixelX_;
    float th = h * pixelY_;

    if (align == TextAlign::None) {
        align = TextAlign::Default;
    }

    if (align & TextAlign::Left) {
        tx -= tw;
    }
    else if (align & TextAlign::Center) {
        tx -= tw / 2;
    }

    if (align & TextAlign::Bottom) {
        ty -= th;
    }
    else if (align & TextAlign::Middle) {
        ty -= th / 2;
    }

    glRasterPos2d(tx, ty);
    glutBitmapString(font, (const unsigned char*)text);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_font(FL_HELVETICA, static_cast<int>(reinterpret_cast<uintptr_t>(font)));

    int w{ 0 }, h{ 0 };
    fl_measure(text, w, h);

    double x0{ get_x(x) }, y0{ get_y(y) };

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

double WaveWidget::get_x(double x) const {
    return (x - PlotDefaults::XMin) / pixelX_ + PlotDefaults::Margin + PlotDefaults::TickSize;
}

double WaveWidget::get_y(double y) const {
    return (PlotDefaults::YMax - y) / pixelY_ + PlotDefaults::Margin;
}
