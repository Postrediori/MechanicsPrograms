// GraphWidget.cpp

#include <FL/gl.h>
#include <FL/glu.h>
#include <FL/glut.H>
#include <cmath>
#include <iostream>
#include "Func.h"
#include "Graph.h"
#include "GraphWidget.h"

static const int XDivCount = 100;
static const int YDivCount = 100;

static const int TickCount = 20;
static const int TickSize = 10;
static const int Margin = 20;

static const int PlotLevels = 10;
static const int PaletteSize = PlotLevels;
static const unsigned char Palette[PaletteSize][4] = {
    {0xf1, 0xed, 0xc8, 0x00},
    {0xe7, 0xdb, 0xa9, 0x00},
    {0xdd, 0xc9, 0x8a, 0x00},
    {0xba, 0xc5, 0x96, 0x00},
    {0xa4, 0xca, 0xa1, 0x00},
    {0xbf, 0xd5, 0xee, 0x00},
    {0xa3, 0xc3, 0xe6, 0x00},
    {0x89, 0xb0, 0xda, 0x00},
    {0x6b, 0x99, 0xc9, 0x00},
    {0x51, 0x82, 0xb4, 0x00},
};
/* static const unsigned int Palette[PaletteSize] = {
    0xf1edc8,
    0xe7dba9,
    0xddc98a,
    0xbac596,
    0xa4caa1,
    0xbfd5ee,
    0xa3c3e6,
    0x89b0da,
    0x6b99c9,
    0x5182b4
}; */

/* static const int PaletteSize = 10;
static const unsigned int Palette[PaletteSize] = {
    0xe20800,
    0xf07113,
    0xffe429,
    0x91a87b,
    0x2c72c7,
    0x175baa,
    0x003876,
    0x003065,
    0x002754,
    0x001e43
}; */

static const GLfloat ColorLevelLines[] = {0.3f, 0.3f, 0.3f};
static const GLfloat ColorTicks[]      = {0.0f, 0.0f, 0.0f};

GraphWidget::GraphWidget(int X, int Y, int W, int H,
                         SearchEngine *engine, const char* l)
 : Fl_Gl_Window(X, Y, W, H, l), engine_(engine),
   ticksX_(NULL), ticksY_(NULL), boundingBox_(NULL),
    contourLines_(NULL), contourFills_(NULL) {

    contourLines_ = new ContourPlot*[PlotLevels];
    for (int i=0; i<PlotLevels; i++)
        contourLines_[i] = new ContourLine();

    contourFills_ = new ContourPlot*[PlotLevels];
    for (int i=0; i<PlotLevels; i++)
        contourFills_[i] = new ContourFill();

    pixelX_ = (XMax - XMin) / (this->w() - Margin*2 - TickSize);
    pixelY_ = (YMax - YMin) / (this->h() - Margin*2 - TickSize);

    create_ticks();
    create_surface();
}

GraphWidget::~GraphWidget() {
    if (ticksX_) {
        delete[] ticksX_;
        ticksX_ = NULL;
    }

    if (ticksY_) {
        delete[] ticksY_;
        ticksY_ = NULL;
    }

    if (boundingBox_) {
        delete[] boundingBox_;
        boundingBox_ = NULL;
    }

    if (contourLines_) {
        for (int i=0; i<PlotLevels; i++)
            delete contourLines_[i];
        delete[] contourLines_;
    }

    if (contourFills_) {
        for (int i=0; i<PlotLevels; i++)
            delete contourFills_[i];
        delete[] contourFills_;
    }
}

void GraphWidget::draw() {
    if (!this->valid()) {
        pixelX_ = (XMax - XMin) / (this->w() - Margin*2 - TickSize);
        pixelY_ = (YMax - YMin) / (this->h() - Margin*2 - TickSize);

        glViewport(0, 0, this->w(), this->h());
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(XMin - pixelX_*(Margin+TickSize),
                XMax + pixelX_*Margin,
                YMin - pixelY_*(Margin+TickSize),
                YMax + pixelY_*Margin);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    if (!this->context_valid()) {
        glShadeModel(GL_SMOOTH);
        glClearColor(1.0, 1.0, 1.0, 0.5);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Draw filled contour plot
    for (int i=0; i<PlotLevels; i++) {
        glColor3ub(Palette[i][0], Palette[i][1], Palette[i][2]);
        contourFills_[i]->render();
    }

    // Draw contour lines
    glColor3fv(ColorLevelLines);
    for (int i=0; i<PlotLevels; i++)
        contourLines_[i]->render();

    // Draw search status
    if (engine_!=NULL) engine_->draw();

    /* Draw box and ticks */
    glColor3fv(ColorTicks);
    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, boundingBox_);
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    /* Draw ticks */
    if (ticksX_) {
        glVertexPointer(2, GL_FLOAT, 0, ticksX_);
        glDrawArrays(GL_LINES, 0, (TickCount + 1) * 2);
    }

    if (ticksY_) {
        glVertexPointer(2, GL_FLOAT, 0, ticksY_);
        glDrawArrays(GL_LINES, 0, (TickCount + 1) * 2);
    }

    glDisableClientState(GL_VERTEX_ARRAY);

    // Print plot ranges
    glRasterPos2d(XMin,
                  YMin - pixelY_ * (Margin / 2 + TickSize));
    PrintText(GLUT_BITMAP_HELVETICA_10, "%.1f", XMin);

    glRasterPos2d(XMax,
                  YMin - pixelY_ * (Margin / 2 + TickSize));
    PrintText(GLUT_BITMAP_HELVETICA_10, "%.1f", XMax);

    glRasterPos2d(XMin - pixelX_ * (Margin / 2 + TickSize),
                  YMax - pixelY_ * (Margin / 2));
    PrintText(GLUT_BITMAP_HELVETICA_10, "%.1f", YMax);

    glRasterPos2d(XMin - pixelX_ * (Margin / 2 + TickSize),
                  YMin + pixelY_ * (Margin / 2));
    PrintText(GLUT_BITMAP_HELVETICA_10, "%.1f", YMin);
}

int GraphWidget::handle(int e) {
    if (e==FL_PUSH) {
        // Set start point for search engines
        int x, y;
        if (Fl::event_button()==FL_LEFT_MOUSE) {
            x = Fl::event_x();
            y = Fl::event_y();
            x -= (Margin + TickSize);
            y -= (Margin);
            // std::cout<<"x: "<<x<<"    y: "<<y<<std::endl;
            if (x>=0 && x<=(this->w() - Margin * 2 - TickSize) &&
                y>=0 && y<=(this->h() - Margin * 2 - TickSize)) {
                float sx, sy;
                sx = XMin + x * pixelX_;
                sy = YMax - y * pixelY_;
                engine_->set_start_point(sx, sy);
                this->redraw();
                this->invalidate();
            }
        }
        return 1;
    }

    return Fl_Gl_Window::handle(e);
}


void GraphWidget::create_ticks() {
    boundingBox_ = new vec2[4];
    boundingBox_[0] = vec2(XMin, YMin);
    boundingBox_[1] = vec2(XMax, YMin);
    boundingBox_[2] = vec2(XMax, YMax);
    boundingBox_[3] = vec2(XMin, YMax);

    float dTick;

    /* Y Ticks */
    ticksY_ = new vec2[(TickCount + 1) * 2];
    dTick = (YMax - YMin) / float(TickCount);
    for (int i=0; i<=TickCount; i++) {
        float y = YMin + i * dTick;
        ticksY_[i * 2].x = XMin;
        ticksY_[i * 2].y = y;

        float tickScale = (i % 2) ? 0.5 : 1.0;
        ticksY_[i * 2 + 1].x = XMin - float(TickSize) * pixelX_ * tickScale;
        ticksY_[i * 2 + 1].y = y;
    }

    /* X Ticks */
    ticksX_ = new vec2[(TickCount + 1) * 2];
    dTick = (XMax - XMin) / float(TickCount);
    for (int i=0; i<=TickCount; i++) {
        float x = XMin + i * dTick;
        ticksX_[i * 2].y = YMin;
        ticksX_[i * 2].x = x;

        float tickScale = (i % 2) ? 0.5 : 1.0;
        ticksX_[i * 2 + 1].y = YMin - float(TickSize) * pixelY_ * tickScale;
        ticksX_[i * 2 + 1].x = x;
    }
}

void GraphWidget::create_surface() {
    int rows = YDivCount + 1, cols = XDivCount + 1;
    float zmin = 0.0f, zmax = 0.0f;
    float dx = (XMax - XMin) / float(cols);
    float dy = (YMax - YMin) / float(rows);
    float *points = new float[rows * cols];

    for (int j=0; j<rows; j++) {
        float y = YMin + j * dy;
        for (int i=0; i<cols; i++) {
            float x = XMin + i * dx;
            float z = func(x, y);
            points[j * cols + i] = z;
            if (zmin>z) zmin = z;
            if (zmax<z) zmax = z;
        }
    }

    float dz = (sqrt(zmax) - sqrt(zmin)) / float (PlotLevels);

    for (int i=0; i<PlotLevels; i++) {
        float z = sqrt(zmin) + float(i) * dz;
        contourLines_[i]->init(points, cols, rows,
            XMin, YMin, XMax, YMax, z*z);
        contourFills_[i]->init(points, cols, rows,
            XMin, YMin, XMax, YMax, z*z);
    }

    delete[] points;
}
