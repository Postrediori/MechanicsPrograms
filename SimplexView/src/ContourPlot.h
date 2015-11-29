// ContourPlot.h
#ifndef CONTOUR_PLOT_H
#define CONTOUR_PLOT_H

#include <vector>

struct vec2 {
    float x, y;

    vec2() : x(0.0f), y(0.0f) { }
    vec2(float x, float y) : x(x), y(y) { }
};

// ----------------------------------------------------------------------------

class ContourPlot {
protected:
    int w_, h_;
    float xmin_, ymin_;
    float xmax_, ymax_;
    float threshold_;

public:
    ContourPlot() : w_(0), h_(0), threshold_(0.0f) { }
    virtual ~ContourPlot() { release(); }

    virtual bool init(
        const float *points, int cols, int rows,
        float xmin, float ymin, float xmax, float ymax,
        float threshold
    ) { return true; }
    virtual void render() { }

    void resize(int w, int h) { w_ = w; h_ = h; }
    void release() { }
};

// ----------------------------------------------------------------------------

class ContourLine: public ContourPlot {
    std::vector<vec2> lines;

public:
    ContourLine() : ContourPlot() { }

    bool init(
        const float *points, int cols, int rows,
        float xmin, float ymin, float xmax, float ymax,
        float threshold);

    void render();
};

// ----------------------------------------------------------------------------

class ContourFill: public ContourPlot {
    std::vector<vec2> triangles;

public:
    ContourFill() : ContourPlot() { }

    bool init(
        const float *points, int cols, int rows,
        float xmin, float ymin, float xmax, float ymax,
        float threshold);

    void render();
};

#endif // CONTOUR_PLOT_H
