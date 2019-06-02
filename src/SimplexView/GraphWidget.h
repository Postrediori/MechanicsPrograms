// GraphWidget.h
#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include "SearchEngine.h"
#include "ContourPlot.h"

class GraphWidget: public Fl_Gl_Window {
    float pixelX_, pixelY_;

    ContourPlot **contourLines_;
    ContourPlot **contourFills_;

    vec2 *boundingBox_;
    vec2 *ticksX_;
    vec2 *ticksY_;

    SearchEngine *engine_;

    virtual void draw();
    int handle(int e);

public:
    GraphWidget(int X, int Y, int W, int H,
                SearchEngine *engine = NULL,
                const char* l = 0);
    virtual ~GraphWidget();

    void create_ticks();
    void create_surface();

    SearchEngine *engine() { return engine_; }
    void engine(SearchEngine *e) { engine_ = e; }
};

#endif
// GRAPH_WIDGET_H
