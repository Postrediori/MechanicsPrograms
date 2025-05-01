#include "pch.h"
#include "MathUtils.h"
#include "GraphUtils.h"
#include "FuncUtils.h"
#include "Simplex.h"
#include "SearchEngine.h"
#include "SimplexEngine.h"

const Fl_Color SimplexColor = fl_rgb_color(0);
const Fl_Color SimplexHistoryColor = fl_rgb_color(128);
const Fl_Color SimplexNode = fl_rgb_color(0);
const Fl_Color SimplexActiveNode = fl_rgb_color(0, 0, 0xff);

void DrawSimplex(CoordinateFunc xFunc, CoordinateFunc yFunc,
        const Simplex&s) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glBegin(GL_LINE_LOOP);
    for (const auto& p : s.points) {
        glVertex2f(xFunc(p.X), yFunc(p.Y));
    }
    glVertex2f(xFunc(s.points[0].X), yFunc(s.points[0].Y));
    glEnd();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_begin_loop();
    for (const auto& p : s.points) {
        fl_vertex(xFunc(p.X), yFunc(p.Y));
    }
    fl_end_loop();
#endif
}

SimplexEngine::SimplexEngine() {
    set_start_point(0.f, -2.5f);
}

void SimplexEngine::draw(CoordinateFunc xFunc, CoordinateFunc yFunc) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    SET_FL_COLOR_TO_GL(SimplexHistoryColor);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(SimplexHistoryColor);
    fl_line_style(FL_SOLID, 1);
#endif
    for (const auto &s : history_) {
        DrawSimplex(xFunc, yFunc, s);
    }

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    SET_FL_COLOR_TO_GL(SimplexColor);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_color(SimplexColor);
    fl_line_style(FL_SOLID, 1);
#endif
    DrawSimplex(xFunc, yFunc, simplex_);

    for (int i=0; i<SimplexOrder; i++) {
        const auto nodeColor = (simplex_.max_node == i) ? SimplexActiveNode : SimplexNode;
#if DRAW_METHOD==DRAW_METHOD_OPENGL
        SET_FL_COLOR_TO_GL(nodeColor);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
        fl_color(nodeColor);
#endif
        constexpr double PointSize = 0.05;
        DrawRectangle(xFunc(simplex_.points[i].X - PointSize),
                      yFunc(simplex_.points[i].Y - PointSize),
                      xFunc(simplex_.points[i].X + PointSize),
                      yFunc(simplex_.points[i].Y + PointSize));
    }
}

void SimplexEngine::search_start() {
    count_ = 0;
    search_over_ = false;

    // float cx = 0.f, cy = -2.5f;
    // set_simplex(simplex_, -4.5, -4.0, -3.5, -4.0, -4.0, sqrt(3.0) / 2.0 - 4.0);
    // set_simplex(simplex_, -0.5+cx, cy, 0.5+cx, cy, cx, sqrt(3.0) / 2.0+cy);
    // set_triangle_simplex(simplex_, 1.0, cx, cy);
    simplex_.setTriangle(1.f, xstart_, ystart_);

    update_min();
    count_ += 3;
    history_.clear();
}

void SimplexEngine::search_step() {
    if (search_over_) {
        return;
    }
    history_.push_back(simplex_);

    int max_node = simplex_.getMaxNode();
    simplex_.flip(max_node);
    count_ += 1;
    if (simplex_.max_node==max_node) {
        simplex_.reduce(max_node);
        count_ += 2;
    }
    simplex_.max_node = max_node;

    update_min();
    if (simplex_.getSize() <= Epsilon) {
        search_over_ = true;
    }
}

void SimplexEngine::update_min() {
    HMM_Vec4 pt = simplex_.getMinPoint();
    xmin_ = pt.X;
    ymin_ = pt.Y;
    zmin_ = pt.Z;
}
