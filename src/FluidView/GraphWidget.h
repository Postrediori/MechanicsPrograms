#pragma once

const int PointCount = 1000;
const int TickCount = 20;

struct point {
    float x, y;
};

class GraphWidget : public Fl_Widget {
public:
    GraphWidget(int X, int Y, int W, int H, const char* l = 0);

    void set_lines(float q, float lambda1, float lambda2);
    void resize(int x, int y, int w, int h) FL_OVERRIDE;

    void draw() FL_OVERRIDE;

private:
    void update_size();

private:
    float pixelX_, pixelY_;

    bool lines_;
    float q_, lambda1_, lambda2_;

    std::vector<point> graph_;

    std::vector<point> ticksX;
    std::vector<point> ticksY;
};
