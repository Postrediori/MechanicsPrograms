
#include <FL/Fl.H>
#include <FL/gl.h>
#include <FL/glu.h>
#include <FL/glut.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Gl_Window.h>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

#include <cstdio>
#include <cstdarg>
#include <cmath>

static const double k = 1.4;
static const double MaxLambda = sqrt((k + 1) / (k - 1));
static const double Epsilon = 1e-3;
static const int MaxIter = 50000;

double qfunc(double lambda) {
    double q1, q2;
    q1 = pow((1 - lambda * lambda * (k - 1) / (k + 1)), 1 / (k - 1));
    q2 = pow((2 / (k + 1)), (-1 / (k - 1)));
    return lambda * q1 * q2;
}

double dqfunc(double lambda) {
    double q1, q2;
    q1 = pow((1 - lambda * lambda * (k - 1) / (k + 1)), 1 / (k - 1) - 1);
    q2 = pow((2 / (k + 1)), (-1 / (k - 1)));
    return (1 - lambda * lambda) * q1 * q2;
}

double lambdafunc(double q, int side) {
    int i;
    double xn, oldxn;

    if (side==1) xn = 0.5;
    else xn = 1.5;

    i = 0;
    while (1) {
        oldxn = xn;
        xn = oldxn - (qfunc(oldxn) - q) / dqfunc(oldxn);
        if ((fabs(xn - oldxn)<Epsilon) || (i>MaxIter)) break;
        i++;
    }

    return xn;
}

///////////////////////////////////////////////////////////////////////////////
const double XMin = 0.0;
const double XMax = MaxLambda;
const double YMin = 0.0;
const double YMax = 1.0;

const int Margin = 20;
const int TickSize = 10;
const int PointCount = 1000;
const int TickCount = 20;

struct point {
    float x, y;
};

void PrintText(void *font, const char *fmt, ...) {
    char text[256];
    va_list ap;

    if (fmt==NULL) return;

    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);

    char *p = (char *) text;
    while (*p!='\0') {
        glutBitmapCharacter(font, *p);
        p++;
    }
}

class GraphWidget: public Fl_Gl_Window {
public:
    GraphWidget(int X, int Y, int W, int H, const char* l = 0);
    void draw();

    void set_lines(float q, float lambda1, float lambda2);

private:
    float pixelX_, pixelY_;

    bool lines_;
    float q_, lambda1_, lambda2_;

    point graph_[PointCount];

    point ticksX[(TickCount + 1) * 2];
    point ticksY[(TickCount + 1) * 2];
};

GraphWidget::GraphWidget(int X, int Y, int W, int H, const char* l)
    : Fl_Gl_Window(X, Y, W, H, l) {
    this->lines_ = false;

    this->pixelX_ = (XMax - XMin) / (this->w() - Margin * 2 - TickSize);
    this->pixelY_ = (YMax - YMin) / (this->h() - Margin * 2 - TickSize);

    double dx = (XMax - XMin) / (double)(PointCount);
    for (int i = 0; i<PointCount; i++) {
        // double x = i / 1000.0 - 2.0;
        double x = XMin + i * dx;
        this->graph_[i].x = x;
        this->graph_[i].y = qfunc(x);
    }

    // Y Ticks
    double dTick = (YMax - YMin) / TickCount;
    for (int i=0; i<=TickCount; i++) {
        double y = YMin + i * dTick;
        ticksY[i * 2].x = XMin;
        ticksY[i * 2].y = y;

        double tickScale = (i % 2) ? 0.5 : 1.0;
        ticksY[i * 2 + 1].x = XMin - TickSize * this->pixelX_ * tickScale;
        ticksY[i * 2 + 1].y = y;
    }

    // X Ticks
    dTick = (XMax - XMin) / TickCount;
    for (int i=0; i<=TickCount; i++) {
        double x = XMin + i * dTick;
        ticksX[i * 2].y = YMin;
        ticksX[i * 2].x = x;

        double tickScale = (i % 2) ? 0.5 : 1.0;
        ticksX[i * 2 + 1].y = YMin - TickSize * this->pixelY_ * tickScale;
        ticksX[i * 2 + 1].x = x;
    }
}

void GraphWidget::draw() {
    if (!this->valid()) {
        this->pixelX_ = (XMax - XMin) / (this->w() - Margin * 2 - TickSize);
        this->pixelY_ = (YMax - YMin) / (this->h() - Margin * 2 - TickSize);

        glViewport(0, 0, this->w(), this->h());
        glScissor(Margin + TickSize, Margin + TickSize,
                  this->w() - Margin * 2 - TickSize, this->h() - Margin * 2 - TickSize);
        glEnable(GL_SCISSOR_TEST);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(XMin - this->pixelX_ * (Margin + TickSize),
                XMax + this->pixelX_ * Margin,
                YMin - this->pixelY_ * (Margin + TickSize),
                YMax + this->pixelY_ * Margin,
                -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    if (!this->context_valid()) {
        glShadeModel(GL_SMOOTH);
        glClearColor(1.0, 1.0, 1.0, 0.5);
        glClearDepth(1.0);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }

    //
    // Draw axis and ticks
    //
    glDisable(GL_SCISSOR_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Draw box
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex2d(XMin, YMin);
    glVertex2d(XMax, YMin);
    glVertex2d(XMax, YMax);
    glVertex2d(XMin, YMax);
    glEnd();

    // Draw ticks
    glBegin(GL_LINES);
    for (int i=0; i<=TickCount; i++) {
        glVertex2d(ticksX[i*2].x, ticksX[i*2].y);
        glVertex2d(ticksX[i*2+1].x, ticksX[i*2+1].y);

        glVertex2d(ticksY[i*2].x, ticksY[i*2].y);
        glVertex2d(ticksY[i*2+1].x, ticksY[i*2+1].y);
    }
    glEnd();

    // Print plot ranges
    glRasterPos2d(XMin,
                  YMin - this->pixelY_ * (Margin / 2 + TickSize));
    PrintText(GLUT_BITMAP_HELVETICA_10, "%.1f", XMin);

    glRasterPos2d(XMax,
                  YMin - this->pixelY_ * (Margin / 2 + TickSize));
    PrintText(GLUT_BITMAP_HELVETICA_10, "%.1f", XMax);

    glRasterPos2d(XMin - this->pixelX_ * (Margin / 2 + TickSize),
                  YMax - this->pixelY_ * (Margin / 2));
    PrintText(GLUT_BITMAP_HELVETICA_10, "%.1f", YMax);

    glRasterPos2d(XMin - this->pixelX_ * (Margin / 2 + TickSize),
                  YMin + this->pixelY_ * (Margin / 2));
    PrintText(GLUT_BITMAP_HELVETICA_10, "%.1f", YMin);

    //
    // Draw graph
    //
    glColor3f(1.0, 0.1, 0.2);
    glBegin(GL_LINE_STRIP);
    for (int i=0; i<PointCount-1; i++) {
        glVertex2d(this->graph_[i].x, this->graph_[i].y);
        glVertex2d(this->graph_[i+1].x, this->graph_[i+1].y);
    }
    glEnd();

    if (this->lines_) {
        glBegin(GL_LINES);
        glColor3f(0.125, 0.5, 0.125);
        glVertex2f(XMin, this->q_);
        glVertex2f(XMax, this->q_);

        glColor3f(0.0, 0.25, 0.75);
        glVertex2f(this->lambda1_, YMin);
        glVertex2f(this->lambda1_, this->q_);

        glVertex2f(this->lambda2_, YMin);
        glVertex2f(this->lambda2_, this->q_);
        glEnd();
    }

    // Draw search status
    /* glColor3f(0.125, 0.5, 0.125);
    glBegin(GL_LINES);
    	glVertex2f(model_->x2(), YMin);
    	glVertex2f(model_->x2(), model_->y2());
    glEnd(); */

    // Draw current search min
    /* glColor3f(0.0, 0.25, 0.75);
    glBegin(GL_QUADS);
    	glVertex2f(model_->x1() - 0.01, model_->y1() - 0.01);
    	glVertex2f(model_->x1() + 0.01, model_->y1() - 0.01);
    	glVertex2f(model_->x1() + 0.01, model_->y1() + 0.01);
    	glVertex2f(model_->x1() - 0.01, model_->y1() + 0.01);
    glEnd(); */

    glFinish();
}

void GraphWidget::set_lines(float q, float lambda1, float lambda2) {
    if (!this->lines_) this->lines_ = true;
    this->q_ = q;
    this->lambda1_ = lambda1;
    this->lambda2_ = lambda2;
}


GraphWidget *graph;
Fl_Input *inpq;
Fl_Button *btnCalc;
Fl_Text_Display *disp;
Fl_Text_Buffer *buffer;

void calcFunc(Fl_Widget *, void *);

int main(int argc, char **argv) {
    Fl_Window *window = new Fl_Window(700, 500, "FluidView");
    window->begin();

    graph = new GraphWidget(10, 10, 480, 480);

    inpq = new Fl_Input(575, 10, 90, 25, "q = ");
    btnCalc = new Fl_Button(500, 40, 90, 25, "Calculate");
    disp = new Fl_Text_Display(500, 70, 190, 200);
    buffer = new Fl_Text_Buffer(1024);
    disp->buffer(buffer);
    btnCalc->callback(calcFunc);

    window->end();
    window->show(argc, argv);

    return Fl::run();
}

void calcFunc(Fl_Widget * /*w*/, void * /*o*/) {
    char text[32];
    strcpy(text, inpq->value());
    double q = atof(text);

    double l1, l2;
    l1 = lambdafunc(q, 1);
    l2 = lambdafunc(q, 2);

    buffer->select(0, buffer->length());
    buffer->remove_selection();

    char str[256];
    sprintf(str, "Lambda1: %12.4f\n"
            "Lambda2: %12.4f\n"
            "Epsilon: %f",
            l1, l2, Epsilon);

    buffer->insert(0, str);

    graph->set_lines(q, l1, l2);
    graph->redraw();
}
