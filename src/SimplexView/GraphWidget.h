#pragma once

#if DRAW_METHOD==DRAW_METHOD_OPENGL
class GraphWidget : public Fl_Gl_Window {
#elif DRAW_METHOD==DRAW_METHOD_FLTK
class GraphWidget : public Fl_Widget {
#endif
public:
    GraphWidget(int X, int Y, int W, int H, SearchEngine* e, const char* l = nullptr);
    ~GraphWidget();

    void create_surface();

    void engine(SearchEngine *e) { engine_ = e; }

    int handle(int e) FL_OVERRIDE;
    void resize(int x, int y, int w, int h) FL_OVERRIDE;

    void draw() FL_OVERRIDE;

private:
    void draw_contour_plot();
    void draw_contour_lines();
    void draw_engine_status();
    void draw_box();
    void draw_ticks();
    void draw_legend();

    void update_size();

private:
    float pixelX_{ 0.0f }, pixelY_{ 0.0f };

    std::vector<ContourLine> contourLines_;
    std::vector<ContourFill> contourFills_;

    std::vector<HMM_Vec2> boundingBox_;
    std::vector<HMM_Vec2> ticksX_;
    std::vector<HMM_Vec2> ticksY_;

    SearchEngine* engine_{ nullptr };

    CoordinateFunc xFunc_, yFunc_;

#if DRAW_METHOD==DRAW_METHOD_FLTK
    bool initOffscreen_{ false };
    Fl_Offscreen offscreen_;
    Fl_Offscreen contourOfs_;

    bool contourOfsNeedsRedraw_{ true };
#endif
};
