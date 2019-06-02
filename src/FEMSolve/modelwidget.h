// modelwidget.h
#ifndef MODEL_WIDGET_H
#define MODEL_WIDGET_H

#include <FL/Fl_Gl_Window.H>
#include "model.h"

static const float Margin = 0.2;
static const float TextMargin = 0.03;

static const float NodeSize = 0.01;
static const float FixSize = 0.05;
static const float FixSize2 = FixSize / 2.0;
static const float ForceSize = 0.15;

static const float FixedColor[3] = {0.25, 0.875, 0.8125};
static const float ElementColor[3] = {0.0, 1.0, 1.0};
static const float TextColor[3] = {1.0, 0.0, 1.0};
static const float NodeColor[3] = {1.0, 1.0, 1.0};
static const float ForceColor[3] = {1.0, 0.65, 0.0};

class ModelWidget: public Fl_Gl_Window {
    void draw();
    FinitModel *model_;
    float xmin_, xmax_, ymin_, ymax_;
    float xsize_, ysize_;

public:
    ModelWidget(int X, int Y, int W, int H, FinitModel *model, const char* l = 0);

    void reload_model();
};

#endif
// MODEL_WIDGET_H
