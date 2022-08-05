#pragma once

#define DRAW_METHOD_OPENGL 1
#define DRAW_METHOD_FLTK 2
#define DRAW_METHOD DRAW_METHOD_FLTK

namespace PlotDefaults {
    static const double XMin = -1.0;
    static const double XMax = 1.0;
    static const double YMin = -1.0;
    static const double YMax = 0.5;

    static const int TickCount = 20;
    static const int TickSize = 15;
    static const int Margin = 40;
}

namespace TextAlign {
    const uint8_t None = 0x00;

    const uint8_t Top = 0x01;
    const uint8_t Middle = 0x02;
    const uint8_t Bottom = 0x04;

    const uint8_t Left = 0x10;
    const uint8_t Center = 0x20;
    const uint8_t Right = 0x40;

    const uint8_t Default = TextAlign::Right | TextAlign::Top;
}

#if DRAW_METHOD==DRAW_METHOD_OPENGL
class WaveWidget : public Fl_Gl_Window {
#elif DRAW_METHOD==DRAW_METHOD_FLTK
class WaveWidget : public Fl_Widget {
#endif
public:
    struct Point2d {
        float x, y;
    };

    WaveWidget(int X, int Y, int W, int H, WaveModel* model = nullptr, const char* l = nullptr);
    ~WaveWidget();

private:
    void draw() override;

    void draw_heatmap();
    void draw_box();
    void draw_axes();
    void draw_model();
    void draw_ticks();
    void draw_legend();

    void draw_text(void* font, double x, double y, uint8_t align, const char* fmt, ...);

    double get_x(double x) const;
    double get_y(double y) const;

private:
    WaveModel* model_{ nullptr };

    double pixelX_{ 0.0 }, pixelY_{ 0.0 };
    double stepX_{ 0.0 }, stepY_{ 0.0 };

    std::vector<Point2d> ticksX;
    std::vector<Point2d> ticksY;

#if DRAW_METHOD==DRAW_METHOD_FLTK
    bool initOffscreen_{ false };
    Fl_Offscreen offscreen_;
#endif
};
