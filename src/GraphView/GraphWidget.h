#pragma once

/******************************************************************************
 * Graph drawing widget
 *****************************************************************************/
const int Margin = 20;
const int TickSize = 10;
const int TickCount = 20;

const int PlotPointsCount = 100;

class GraphWidget: public Fl_Widget {
public:
    GraphWidget(int x, int y, int w, int h, const char *lbl = nullptr);
    ~GraphWidget();

    void resize(int x, int y, int w, int h) FL_OVERRIDE;

    void draw() FL_OVERRIDE;

    void set_model(const std::unique_ptr<Model>& m) { this->model_ = m.get(); }

    void set_plot(Function1D func, Area functionArea);

private:
    void release_offscreen();
    void update_offscreen();

    void update_scale();
    void update_plot(Function1D func);
    void update_ticks();

private:
    CoordinateFunction coordFunc_;

    Area area_ = {-1.0, 1.0, -1.0, 1.0};
    Model *model_ = nullptr;

    double pixelX_ = 0.0, pixelY_ = 0.0;

    std::vector<Line> plotLines_;

    std::vector<Line> ticksX_;
    std::vector<Line> ticksY_;

    bool offscreenCreated_ = false;
    Fl_Offscreen offscreen_;
};
