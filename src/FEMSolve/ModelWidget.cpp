#include "pch.h"
#include "LinAlgUtils.h"
#include "FEM.h"
#include "GraphicsUtils.h"
#include "ModelWidget.h"

const Fl_Color TitleColor = fl_rgb_color(255);
const Fl_Color FixedColor = fl_rgb_color(64, 224, 208);
const Fl_Color ElementColor = fl_rgb_color(0, 255, 255);
const Fl_Color TextColor = fl_rgb_color(255, 0, 255);
const Fl_Color NodeColor = fl_rgb_color(255);
const Fl_Color ForceColor = fl_rgb_color(255, 165, 0);

#if DRAW_METHOD==DRAW_METHOD_OPENGL
#define SET_FL_COLOR_TO_GL(c) { \
    GLubyte r, g, b, a; \
    Fl::get_color((c), r, g, b, a); \
    glColor4ub(r, g, b, a); \
    }
#endif

ModelWidget::ModelWidget(int X, int Y, int W, int H, FinitModel& model, const char* l)
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    : Fl_Gl_Window(X, Y, W, H, l)
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    : Fl_Widget(X, Y, W, H, l)
#endif
    , model_(model) {

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    xFunc = [=](float x) -> float {
        return x;
    };
    yFunc = [=](float y) -> float {
        return y;
    };
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    xFunc = [=](float x) -> float {
        return (x - this->xmin_) / this->pixelX_;
    };
    yFunc = [=](float y) -> float {
        return (this->ymax_ - y) / this->pixelY_;
    };
#endif

    update_size();
}

ModelWidget::~ModelWidget() {
#if DRAW_METHOD==DRAW_METHOD_FLTK
    if (initOffscreen_) {
        fl_delete_offscreen(offscreen_);
    }
#endif
}

void ModelWidget::resize(int x, int y, int w, int h) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    Fl_Gl_Window::resize(x, y, w, h);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    Fl_Widget::resize(x, y, w, h);
#endif
    update_size();
}

void ModelWidget::reload_model() {
    using namespace PlotDefaults;

    if (model_.nodes.size()==0) {
        xmin_ = XMin;
        xmax_ = XMax;
        ymin_ = YMin;
        ymax_ = YMax;
        this->redraw();
        return;
    }

    double xmin, xmax, ymin, ymax;
    xmin = xmax = model_.nodes[0].x;
    ymin = ymax = model_.nodes[0].y;
    for (int i=1; i<model_.nodes.size(); i++) {
        Node n = model_.nodes[i];
        xmin = (n.x < xmin) ? n.x : xmin;
        xmax = (n.x > xmax) ? n.x : xmax;
        ymin = (n.y < ymin) ? n.y : ymin;
        ymax = (n.y > ymax) ? n.y : ymax;
    }

    xsize_ = xmax - xmin;
    ysize_ = ymax - ymin;
    if (xsize_==0.0) xsize_ = 1.0;
    if (ysize_==0.0) ysize_ = 1.0;

    double xmargin = xsize_ * Margin;
    double ymargin = ysize_ * Margin;

    xmin_ = xmin - xmargin;
    xmax_ = xmax + xmargin;
    ymin_ = ymin - ymargin;
    ymax_ = ymax + ymargin;

    update_size();

    this->redraw();
}

void ModelWidget::draw() {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    if (!this->valid()) {
        glViewport(0, 0, this->w(), this->h());
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(xmin_, xmax_, ymin_, ymax_, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
#endif

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    if (!this->context_valid()) {
        glShadeModel(GL_SMOOTH);
        glClearColor(0.0, 0.0, 0.0, 0.5);
        glClearDepth(1.0);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    if (!initOffscreen_) {
        initOffscreen_ = true;
        offscreen_ = fl_create_offscreen(this->w(), this->h());
    }
#endif

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_begin_offscreen(offscreen_);

    fl_color(fl_rgb_color(0));
    fl_rectf(0, 0, this->w(), this->h());
#endif

    draw_legend();
    draw_elements();
    draw_fixes();
    draw_nodes();
    draw_loads();

#if DRAW_METHOD==DRAW_METHOD_FLTK
    fl_end_offscreen();

    fl_copy_offscreen(this->x(), this->y(), this->w(), this->h(),
        offscreen_, 0, 0);
#endif
}

void ModelWidget::draw_legend() {
    using namespace PlotDefaults;

    // Print model title
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    SET_FL_COLOR_TO_GL(TitleColor);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(TitleColor);
#endif
    PrintText(largeFont,
        xFunc(xmin_ + xsize_ * TextMargin * 1.5),
        yFunc(ymax_ - ysize_ * TextMargin * 1.5),
        "TITLE: %s", model_.title.c_str());
}

void ModelWidget::draw_elements() {
    using namespace PlotDefaults;

    for (int i = 0; i < model_.elems.size(); i++) {
        Element e = model_.elems[i];
        Node n1 = model_.nodes[e.nodes[0] - 1], n2 = model_.nodes[e.nodes[1] - 1];

#if DRAW_METHOD==DRAW_METHOD_OPENGL
        SET_FL_COLOR_TO_GL(ElementColor);
        glLineWidth(1.0);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
        fl_color(ElementColor);
        fl_line_style(FL_SOLID, 1);
#endif
        DrawLine(
            xFunc(n1.x), yFunc(n1.y),
            xFunc(n2.x), yFunc(n2.y));

#if DRAW_METHOD==DRAW_METHOD_OPENGL
        SET_FL_COLOR_TO_GL(TextColor);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
        fl_color(TextColor);
#endif
        PrintText(normalFont,
            xFunc((n1.x + n2.x) * 0.5 - xsize_ * TextMargin),
            yFunc((n1.y + n2.y) * 0.5 + ysize_ * TextMargin),
            "%d", e.elem);
    }
}

void ModelWidget::draw_fixes() {
    using namespace PlotDefaults;

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    SET_FL_COLOR_TO_GL(FixedColor);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(FixedColor);
#endif
    for (int i = 0; i < model_.fixes.size(); i++) {
        Fixture f = model_.fixes[i];
        Node n = model_.nodes[f.node - 1];
        if (f.axis == 1) {
            // X
            DrawTriangle(xFunc(n.x), yFunc(n.y),
                xFunc(n.x - xsize_ * FixSize), yFunc(n.y + ysize_ * FixSize2),
                xFunc(n.x - xsize_ * FixSize), yFunc(n.y - ysize_ * FixSize2));
        }
        else if (f.axis == 2) {
            // Y
            DrawTriangle(xFunc(n.x), yFunc(n.y),
                xFunc(n.x - xsize_ * FixSize2), yFunc(n.y - ysize_ * FixSize),
                xFunc(n.x + xsize_ * FixSize2), yFunc(n.y - ysize_ * FixSize));
        }
    }
}

void ModelWidget::draw_nodes() {
    using namespace PlotDefaults;

    for (int i = 0; i < model_.nodes.size(); i++) {
        Node n = model_.nodes[i];

#if DRAW_METHOD==DRAW_METHOD_OPENGL
        SET_FL_COLOR_TO_GL(NodeColor);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
        fl_color(NodeColor);
#endif
        DrawRectangle(xFunc(n.x - xsize_ * NodeSize), yFunc(n.y - ysize_ * NodeSize),
            xFunc(n.x + xsize_ * NodeSize), yFunc(n.y + ysize_ * NodeSize));

#if DRAW_METHOD==DRAW_METHOD_OPENGL
        SET_FL_COLOR_TO_GL(TextColor);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
        fl_color(TextColor);
#endif
        PrintText(normalFont,
            xFunc(n.x - xsize_ * (NodeSize + TextMargin)),
            yFunc(n.y - ysize_ * (NodeSize + TextMargin)),
            "%d", n.node);
    }
}

void ModelWidget::draw_loads() {
    using namespace PlotDefaults;

    // Forces and loads
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    SET_FL_COLOR_TO_GL(ForceColor);
    glLineWidth(3.0);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(ForceColor);
    fl_line_style(FL_SOLID, 3);
#endif
    for (int i = 0; i < model_.loads.size(); i++) {
        Load l = model_.loads[i];
        Node n = model_.nodes[l.node - 1];
        double angle = atan2(l.py, l.px);
        float cosa = cos(angle);
        float sina = sin(angle);
        DrawArrow(xFunc(n.x - xsize_ * ForceSize * cosa), yFunc(n.y - ysize_ * ForceSize * sina),
            xFunc(n.x), yFunc(n.y));
    }
}

void ModelWidget::update_size() {
    // Update pixel scales
    pixelX_ = (xmax_ - xmin_) / static_cast<float>(this->w());
    pixelY_ = (ymax_ - ymin_) / static_cast<float>(this->h());

#if DRAW_METHOD==DRAW_METHOD_FLTK
    if (initOffscreen_) {
        fl_delete_offscreen(offscreen_);
    }

    offscreen_ = fl_create_offscreen(this->w(), this->h());
    initOffscreen_ = true;
#endif
}
