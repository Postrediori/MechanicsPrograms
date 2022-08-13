#pragma once

namespace PlotDefaults {
    constexpr float XMin = -1.0;
    constexpr float XMax = 1.0;
    constexpr float YMin = -1.0;
    constexpr float YMax = 1.0;

    constexpr float Margin = 0.2;
    constexpr float TextMargin = 0.03;

    constexpr float NodeSize = 0.01;
    constexpr float FixSize = 0.05;
    constexpr float FixSize2 = FixSize / 2.0;
    constexpr float ForceSize = 0.15;
}

class ModelWidget :
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    public Fl_Gl_Window {
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    public Fl_Widget{
#endif
public:
    ModelWidget(int X, int Y, int W, int H, FinitModel & model, const char* l = 0);
    ~ModelWidget();

    void resize(int x, int y, int w, int h) override;

    void reload_model();

protected:
    void draw() override;

private:
    void draw_legend();
    void draw_elements();
    void draw_fixes();
    void draw_nodes();
    void draw_loads();

    void update_size();

private:
    FinitModel& model_;
    float xmin_{ PlotDefaults::XMin }, xmax_{ PlotDefaults::XMax };
    float ymin_{ PlotDefaults::YMin }, ymax_{ PlotDefaults::YMax };
    float xsize_{ 0 }, ysize_{ 0 };
    float pixelX_{ 0 }, pixelY_{ 0 };

    CoordinateFunc xFunc, yFunc;

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    void* normalFont{ GLUT_BITMAP_HELVETICA_12 };
    void* largeFont{ GLUT_BITMAP_HELVETICA_18 };
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    bool initOffscreen_{ false };
    Fl_Offscreen offscreen_;

    int normalFont{ 12 };
    int largeFont{ 18 };
#endif
};
