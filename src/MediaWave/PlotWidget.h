#pragma once

#define DRAW_METHOD_OPENGL 1
#define DRAW_METHOD_FLTK 2
#ifndef DRAW_METHOD
#define DRAW_METHOD DRAW_METHOD_FLTK
#endif

namespace PlotDefaults {
    const float XMin = -1.0;
    const float XMax = 1.0;
    const float YMin = -1.0;
    const float YMax = 1.0;

    const float XAxis = 0.0;
    const float YAxis = 0.0;

    const int Margin = 25;
    const int TickSize = 10;
    const int TickCount = 20;
}

namespace TextAlign {
    const uint8_t Unspecified = 0x00;

    const uint8_t Top = 0x01;
    const uint8_t Middle = 0x02;
    const uint8_t Bottom = 0x04;

    const uint8_t Left = 0x10;
    const uint8_t Center = 0x20;
    const uint8_t Right = 0x40;

    const uint8_t Default = TextAlign::Right | TextAlign::Top;
}

#if DRAW_METHOD==DRAW_METHOD_OPENGL
class PlotWidget : public Fl_Gl_Window {
#elif DRAW_METHOD==DRAW_METHOD_FLTK
class PlotWidget : public Fl_Widget {
#endif
public:
    struct Point2D {
        float x, y;
    };

    PlotWidget(int X, int Y, int W, int H, const char* lbl = nullptr);
    ~PlotWidget();

    void resize(int x, int y, int w, int h) FL_OVERRIDE;

    void draw() FL_OVERRIDE;

    void view_range(float, float, float, float);
    void ticks(int, int);
    void plot(int, const std::vector<float>&, const std::vector<float>&);

    void bar_plot(bool b) { bar_plot_ = b; }

private:
    void pixel_scale();

    void draw_box();
    void draw_ticks();
    void draw_axis();
    void draw_heatmap();
    void draw_plot();
    void draw_legend();

    float get_x(float x) const;
    float get_y(float y) const;

    void print_text(float, float, uint8_t, const char*, ...);

    void axis(float, float);
    void margin(int);

private:
    float xmin_, xmax_, ymin_, ymax_;
    float xaxis_, yaxis_;
    int tick_count_, tick_size_, margin_;

    float pixel_x, pixel_y;
    std::vector<Point2D> ticks_x;
    std::vector<Point2D> ticks_y;

    int point_count_;
    std::vector<Point2D> points;

    Fl_Color tick_color;
    Fl_Color axis_color;
    Fl_Color plot_color;
    Fl_Color text_color;

    bool bar_plot_{ false };

#if DRAW_METHOD==DRAW_METHOD_FLTK
    bool initOffscreen_{ false };
    Fl_Offscreen offscreen_;
#endif
};
