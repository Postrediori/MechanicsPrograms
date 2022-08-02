#pragma once

struct Point2D {
    float x, y;
};

const float DefXMin = -1.0;
const float DefXMax = 1.0;
const float DefYMin = -1.0;
const float DefYMax = 1.0;

const float DefXAxis = 0.0;
const float DefYAxis = 0.0;

const int DefMargin = 25;
const int DefTickSize = 10;
const int DefTickCount = 20;

class PlotWidget : public Fl_Gl_Window {
private:
    float xmin_, xmax_, ymin_, ymax_;
    float xaxis_, yaxis_;
    int tick_count_, tick_size_, margin_;

    float pixel_x, pixel_y;
    std::vector<Point2D> ticks_x;
    std::vector<Point2D> ticks_y;

    int point_count_;
    std::vector<Point2D> points;

    float tick_color[3];
    float axis_color[3];
    float plot_color[3];
    float text_color[3];

    void pixel_scale();

    void draw_box();
    void draw_ticks();
    void draw_axis();
    void draw_heatmap();
    void draw_plot();
    void draw_legend();

    virtual void draw();

public:
    PlotWidget(int X, int Y, int W, int H, const char* lbl = nullptr);

    void print_text(void*, float, float, unsigned char, const char*, ...);

    void axis(float, float);
    void view_range(float, float, float, float);
    void ticks(int, int);
    void margin(int);
    void plot(int, const std::vector<float>&, const std::vector<float>&);
};
