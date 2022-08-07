#pragma once

// ----------------------------------------------------------------------------

class ContourPlot {
public:
    ContourPlot() = default;
    virtual ~ContourPlot() { }

    virtual bool init(
        const std::vector<float>& /*points*/, int /*cols*/, int /*rows*/,
        float /*xmin*/, float /*ymin*/, float /*xmax*/, float /*ymax*/,
        float /*threshold*/
    ) { return true; }
    virtual void render(CoordinateFunc xFunc, CoordinateFunc yFunc) const { }

    void resize(int w, int h) { w_ = w; h_ = h; }

protected:
    int w_{ 0 }, h_{ 0 };
    float xmin_{ 0.0f }, ymin_{ 0.0f };
    float xmax_{ 0.0f }, ymax_{ 0.0f };
    float threshold_{ 0.0f };
};

// ----------------------------------------------------------------------------

class ContourLine: public ContourPlot {
public:
    ContourLine() = default;

    bool init(
        const std::vector<float>& points, int cols, int rows,
        float xmin, float ymin, float xmax, float ymax,
        float threshold) override;

    void render(CoordinateFunc xFunc, CoordinateFunc yFunc) const override;

private:
    std::vector<vec2> lines;
};

// ----------------------------------------------------------------------------

class ContourFill: public ContourPlot {
public:
    ContourFill() = default;

    bool init(
        const std::vector<float>& points, int cols, int rows,
        float xmin, float ymin, float xmax, float ymax,
        float threshold) override;

    void render(CoordinateFunc xFunc, CoordinateFunc yFunc) const override;

private:
    std::vector<vec2> triangles;
};
