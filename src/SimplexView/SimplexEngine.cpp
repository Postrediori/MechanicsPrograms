// SimplexEngine.cpp

#include <FL/gl.h>
#include <cmath>
#include "Graph.h"
#include "SimplexEngine.h"

static const GLfloat SimplexColor[]        = {0.f, 0.f, 0.f, 1.f};
static const GLfloat SimplexHistoryColor[] = {.5f, .5f, .5f, 1.f};
static const GLfloat SimplexNode[]         = {0.f, 0.f, 0.f, 1.f};
static const GLfloat SimplexActiveNode[]   = {0.f, 0.f, 1.f, 1.f};

void DrawSimplex(const SIMPLEX &s) {
    glBegin(GL_LINE_LOOP);
    for (int i=0; i<SimplexOrder; i++)
        glVertex2f(s.points[i].x, s.points[i].y);
    glVertex2f(s.points[0].x, s.points[0].y);
    glEnd();
}

SimplexEngine::SimplexEngine() {
    set_start_point(0.f, -2.5f);
}

void SimplexEngine::draw() {
    glColor4fv(SimplexHistoryColor);
    for (int i=0; i<history_.size(); i++)
        DrawSimplex(history_[i]);

    glColor4fv(SimplexColor);
    DrawSimplex(simplex_);

    for (int i=0; i<SimplexOrder; i++) {
        if (simplex_.max_node==i) glColor4fv(SimplexActiveNode);
        else glColor4fv(SimplexNode);
        DrawRectangle(simplex_.points[i].x-0.05,
                      simplex_.points[i].y-0.05,
                      simplex_.points[i].x+0.05,
                      simplex_.points[i].y+0.05);
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
    if (search_over_) return;
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
    if (simplex_.getSize()<=Epsilon) search_over_ = true;
}

void SimplexEngine::update_min() {
    SIMPLEX_POINT pt = simplex_.getMinPoint();
    xmin_ = pt.x;
    ymin_ = pt.y;
    zmin_ = pt.z;
}
