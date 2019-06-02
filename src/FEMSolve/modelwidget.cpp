// modelwidget.cpp

#include <FL/fl_draw.H>
#include <FL/gl.h>
#include <FL/glut.H>
#include <math.h>
#include "graphics.h"
#include "modelwidget.h"
#include <FL/fl_draw.H>

ModelWidget::ModelWidget(int X, int Y, int W, int H, FinitModel *model, const char* l)
        : Fl_Gl_Window(X, Y, W, H, l), model_(model),
        xmin_(-2.0), xmax_(2.0), ymin_(-2.0), ymax_(2.0) {
    //
}


void ModelWidget::reload_model() {
    if (model_->node_count==0) {
        xmin_ = -1.0;
        xmax_ = 1.0;
        ymin_ = -1.0;
        ymax_ = 1.0;
        this->invalidate();
        return;
    }

    double xmin, xmax, ymin, ymax;
    xmin = xmax = model_->nodes[0].x;
    ymin = ymax = model_->nodes[0].y;
    for (int i=1; i<model_->node_count; i++) {
        NODE n = model_->nodes[i];
        if (xmin>n.x) xmin = n.x;
        if (xmax<n.x) xmax = n.x;
        if (ymin>n.y) ymin = n.y;
        if (ymax<n.y) ymax = n.y;
    }

    xsize_ = xmax - xmin;
    ysize_ = ymax - ymin;
    if (xsize_==0.0) xsize_ = 1.0;
    if (ysize_==0.0) ysize_ = 1.0;

    double xmargin, ymargin;
    xmargin = xsize_ * Margin;
    ymargin = ysize_ * Margin;

    xmin_ = xmin - xmargin;
    xmax_ = xmax + xmargin;
    ymin_ = ymin - ymargin;
    ymax_ = ymax + ymargin;

    this->invalidate();
}

void ModelWidget::draw() {
    if (!this->valid()) {
        glViewport(0, 0, this->w(), this->h());
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(xmin_, xmax_, ymin_, ymax_, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    if (!this->context_valid()) {
        glShadeModel(GL_SMOOTH);
        glClearColor(0.0, 0.0, 0.0, 0.5);
        glClearDepth(1.0);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    int i;
	
	// Print model title
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(xmin_ + xsize_*TextMargin*1.5, ymax_ - ysize_*TextMargin*1.5);
	PrintText(GLUT_BITMAP_HELVETICA_12, "TITLE: %s", model_->title);

    // Elements
    for (i=0; i<model_->elem_count; i++) {
        ELEM e = model_->elems[i];
        NODE n1 = model_->nodes[e.nodes[0]-1], n2 = model_->nodes[e.nodes[1]-1];

        glColor3fv(ElementColor);
        DrawLine(n1.x, n1.y, n2.x, n2.y);

        glColor3fv(TextColor);
        glRasterPos2d((n1.x+n2.x)*0.5 - xsize_*TextMargin,
                      (n1.y+n2.y)*0.5 + ysize_*TextMargin);
        PrintText(GLUT_BITMAP_HELVETICA_12, "%d", e.elem);
    }

    // Fixed
    glColor3fv(FixedColor);
    for (i=0; i<model_->fixed_count; i++) {
        FIX f = model_->fixed[i];
        NODE n = model_->nodes[f.node-1];
        if (f.axis==1) {
            // X
            glBegin(GL_TRIANGLES);
            glVertex2f(n.x, n.y);
            glVertex2f(n.x - xsize_ * FixSize, n.y + ysize_ * FixSize2);
            glVertex2f(n.x - xsize_ * FixSize, n.y - ysize_ * FixSize2);
            glEnd();

        } else if (f.axis==2) {
            // Y
            glBegin(GL_TRIANGLES);
            glVertex2f(n.x, n.y);
            glVertex2f(n.x - xsize_ * FixSize2, n.y - ysize_ * FixSize);
            glVertex2f(n.x + xsize_ * FixSize2, n.y - ysize_ * FixSize);
            glEnd();

        }
    }

    // Nodes
    for (i=0; i<model_->node_count; i++) {
        NODE n = model_->nodes[i];

        glColor3fv(NodeColor);
        DrawRectangle(n.x - xsize_ * NodeSize, n.y - ysize_ * NodeSize,
                      n.x + xsize_ * NodeSize, n.y + ysize_ * NodeSize);

        glColor3fv(TextColor);
        glRasterPos2d(n.x - xsize_ * (NodeSize+TextMargin), n.y - ysize_ * (NodeSize+TextMargin));
        PrintText(GLUT_BITMAP_HELVETICA_10, "%d", n.node);
    }

    // Forces and loads
    glColor3fv(ForceColor);
    glLineWidth(3.0);
    for (i=0; i<model_->load_count; i++) {
        LOAD l = model_->loads[i];
        NODE n = model_->nodes[l.node-1];
        double angle = atan2(l.py, l.px);
        float cosa = cos(angle);
        float sina = sin(angle);
        DrawArrow(n.x - xsize_ * ForceSize * cosa, n.y - ysize_ * ForceSize * sina, n.x, n.y);
    }
    glLineWidth(1.0);
}
