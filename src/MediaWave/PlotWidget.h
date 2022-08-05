#pragma once

#define DRAW_OPENGL

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

#ifdef DRAW_OPENGL
class PlotWidget : public Fl_Gl_Window {
#else
class PlotWidget : public Fl_Widget {
#endif
public:
    PlotWidget(int X, int Y, int W, int H, const char* lbl = nullptr);

    void view_range(float, float, float, float);
    void ticks(int, int);
    void plot(int, const std::vector<float>&, const std::vector<float>&);

private:
    void pixel_scale();

    void draw_box();
    void draw_ticks();
    void draw_axis();
    void draw_heatmap();
    void draw_plot();
    void draw_legend();

    void draw() override;

    float get_x(float x) const {
        return (x - xmin_) / pixel_x + margin_ + tick_size_ + this->x();
    }

    float get_y(float y) const {
        return (ymax_ - y) / pixel_y + margin_ + this->y();
    }

#ifdef DRAW_OPENGL
    void print_text(void*, float, float, uint8_t, const char*, ...);
#endif

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

    std::array<float, 4> tick_color;
    std::array<float, 4> axis_color;
    std::array<uint8_t, 4> plot_color;
    std::array<float, 4> text_color;
};
