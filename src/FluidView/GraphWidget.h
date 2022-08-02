#pragma once

const int PointCount = 1000;
const int TickCount = 20;

struct point {
    float x, y;
};

class GraphWidget : public Fl_Widget {
public:
    GraphWidget(int X, int Y, int W, int H, const char* l = 0);
    void draw();

    void set_lines(float q, float lambda1, float lambda2);

private:
    float pixelX_, pixelY_;

    bool lines_;
    float q_, lambda1_, lambda2_;

    point graph_[PointCount];

    point ticksX[(TickCount + 1) * 2];
    point ticksY[(TickCount + 1) * 2];
};
