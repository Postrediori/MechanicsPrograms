/*****************************************************************************
 * GraphView
 * Version 6
 * 11.04.2012
 * main.cpp
 ****************************************************************************/

#include "pch.h"

/*****************************************************************************
 * Constants
 ****************************************************************************/
const double XMin = -2.0;
const double XMax = -1.0;
const double YMin = -1.0;
const double YMax = 0.0;
const double Epsilon = 1e-3;
double TimerInterval = 0.0025;

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


/*****************************************************************************
 * Model routines
 ****************************************************************************/

//
// Function to evaluate
//
double func(double x) {
    // return exp(x) + 1 / x;
    return sin(x) + 0.5;
    // return sin(x * 10.0) / 5.0 - 2.0 - x;
}

/******************************************************************************
 * Base model class
 *****************************************************************************/
using FUNC1D = std::function<double(double)>; // Pointer to one-dimensional function

class Model {
public:
    Model() = default;
    virtual ~Model() { }
    
    virtual void start() { }
    virtual void step() { }
    virtual double timer_interval() const { return 0.025; }

    void stop() { search_ = false; }

    bool search() const { return search_; }
    int counter() const { return counter_; }

    double x1() const { return x1_; }
    double x2() const { return x2_; }
    double y1() const { return y1_; }
    double y2() const { return y2_; }

    void init_search(double min, double max, double e, FUNC1D f) {
        search_ = false;
        counter_ = 0;

        min_ = min;
        max_ = max;
        e_ = e;
        f_ = f;
    }

    void reset() {
        x1_ = x2_ = 0.0;
        y1_ = y2_ = 0.0;
    }

protected:
    bool search_ = false;
    int counter_ = 0;

    double min_ = 0.0, max_ = 0.0;
    double e_ = 0.0;
    FUNC1D f_;

    double x1_ = 0.0, x2_ = 0.0;
    double y1_ = 0.0, y2_ = 0.0;
};

/******************************************************************************
 * Model for scaning method
 *****************************************************************************/
class ScanModel: public Model {
public:
    ScanModel() = default;
    void start();
    void step();
    
    double timer_interval() const { return 0.0025; }

protected:
    double dx_ = 0.0;
};

void ScanModel::start() {
    search_ = true;
    counter_ = 0;
    
    double len = max_ - min_;
    int N = 2 * len / e_ - 1;
    dx_ = len / (double)(N);

    x1_ = x2_ = min_;
    y1_ = y2_ = f_(min_);
    counter_++;
}

void ScanModel::step() {
    if (!search_) return;

    x2_ += dx_;
    if (x2_>max_) {
        x2_ = max_;
        stop();
    }

    y2_ = f_(x2_);
    counter_++;

    if (y2_<y1_) {
        x1_ = x2_;
        y1_ = y2_;
    }
}

/******************************************************************************
 * Model for Fibbonacci method
 *****************************************************************************/
class FibModel: public Model {
public:
    FibModel() = default;
    
    void start();
    void step();
    double timer_interval() const { return 1.0; }

protected:
    int fib_[50] = { 1 };
    int k_ = 0;

    double dm_ = 0.0;
};

void FibModel::start() {
    search_ = true;
    counter_ = 0;
    
    double len = max_ - min_;
    int N = int(len / e_);

    int k = 2;
    while (N>fib_[k-1]) {
        fib_[k] = fib_[k-1] + fib_[k-2];
        k++;
    }

    k_ = k - 1;
    dm_ = len / double(fib_[k_]);

    x1_ = x2_ = min_;
    y1_ = y2_ = f_(min_);
    k_ -= 2;
    counter_++;
}

void FibModel::step() {
    if (!search_) return;
    
    if (k_<=1) {
        stop();
        return;
    }

    x2_ = x1_;
    y2_ = y1_;

    double x1 = x1_+dm_*fib_[k_];
    double y1 = f_(x1), y2 = f_(x1_);

    if (y1<y2) x1_ = x1+dm_*fib_[k_-1];
    else x1_ -= dm_*fib_[k_-1];

    y2_ = y2;
    k_ -= 2;
    counter_ += 2;

    y1_ = f_(x1_);

}

/******************************************************************************
 * Model for Gradient method
 *****************************************************************************/
class GradModel: public Model {
public:
    GradModel() = default;
    
    void start();
    void step();
    double timer_interval() const { return 0.1; }

protected:
    double dx_ = 0.0;
};

void GradModel::start() {
    search_ = true;
    counter_ = 0;

    dx_ = 100.0 * e_;
    
    x1_ = min_;
    y1_ = f_(x1_);

    x2_ = min_ + dx_;
    y2_ = f_(x2_);

    counter_ += 2;
}

void GradModel::step() {
    if (!search_) return;
    
    if (y1_<=y2_) {
        stop();
        return;
    }

    double x = x2_-dx_*(y2_-y1_)/(x2_-x1_);

    if (x>max_) {
        x1_ = max_;
        y1_ = f_(x1_);
        counter_++;
        
        stop();
        return;
    }

    x1_ = x2_; y1_ = y2_;
    x2_ = x; y2_ = f_(x);
    counter_++;
}

/******************************************************************************
 * Model for Bounce method
 *****************************************************************************/
class BounceModel: public Model {
public:
    BounceModel() = default;
    
    void start();
    void step();
    double timer_interval() const { return 0.1; }

protected:
    double dx_ = 0.0;
};

void BounceModel::start() {
    search_ = true;
    counter_ = 0;

    dx_ = 100.0 * e_;
    
    x1_ = min_;
    y1_ = f_(x1_);

    x2_ = min_ + dx_;
    y2_ = f_(x2_);

    counter_ += 2;
}

void BounceModel::step() {
    if (!search_) return;
    
    if (y1_<=y2_) {
        stop();
        return;
    }

    double x = x2_-dx_*(y2_-y1_)/(x2_-x1_) + dx_ * (x2_ - x1_);

    if (x>max_) {
        x1_ = max_;
        y1_ = f_(x1_);
        counter_++;
        
        stop();
        return;
    }

    x1_ = x2_; y1_ = y2_;
    x2_ = x; y2_ = f_(x);
    counter_++;
}

/******************************************************************************
 * Model for Boltsano method
 *****************************************************************************/
class BolzanoModel: public Model {
public:
    BolzanoModel() = default;

    void start();
    void step();
    double timer_interval() const { return 0.5; }

protected:
    double l_ = 0.0;
    double a_ = 0.0, b_ = 0.0;
};

void BolzanoModel::start() {
    search_ = true;
    counter_ = 0;

    a_ = min_;
    b_ = max_;
    l_ = (b_ - a_) / 2.0;
}

void BolzanoModel::step(){
    if (!search_) return;
    
    if (l_<=e_) {
        stop();
        return;
    }

    x1_ = (a_ + b_) / 2.0;
    if (f_(x1_+e_)>=f_(x1_-e_)) {
        b_ = x1_;
        a_ = a_;
    } else {
        a_ = x1_;
        b_ = b_;
    }
    l_ = (b_ - a_) / 2.0;

    x2_ = x1_;
    y2_ = y1_ = f_(x1_);

    counter_ += 2;
}

/******************************************************************************
 * Graph drawing widget
 *****************************************************************************/
const int Margin = 20;
const int TickSize = 10;
const int PointCount = 1000;
const int TickCount = 20;

struct point {
    float x, y;
};

class GraphWidget: public Fl_Gl_Window {
public:
    GraphWidget(int x, int y, int w, int h, Model *m, const char *lbl = NULL);
    void draw();

    void model(Model *m) { this->model_ = m; }
    Model *model() { return this->model_; }

private:
    Model *model_;

    double pixelX_, pixelY_;

    point graph_[PointCount];

    point ticksX[(TickCount + 1) * 2];
    point ticksY[(TickCount + 1) * 2];
};

GraphWidget::GraphWidget(int x, int y, int w, int h, Model *m, const char *lbl)
    : Fl_Gl_Window(x, y, w, h, lbl) {
    this->model_ = m;

    this->pixelX_ = (XMax - XMin) / (this->w() - Margin * 2 - TickSize);
    this->pixelY_ = (YMax - YMin) / (this->h() - Margin * 2 - TickSize);

    for (int i = 0; i<PointCount; i++) {
        double x = i / 1000.0 - 2.0;
        this->graph_[i].x = x;
        this->graph_[i].y = func(x);
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

    // Draw search status
    glColor3f(0.125, 0.5, 0.125);
    glBegin(GL_LINES);
        glVertex2f(model_->x2(), YMin);
        glVertex2f(model_->x2(), model_->y2());
    glEnd();

    // Draw current search min
    glColor3f(0.0, 0.25, 0.75);
    glBegin(GL_QUADS);
        glVertex2f(model_->x1() - 0.01, model_->y1() - 0.01);
        glVertex2f(model_->x1() + 0.01, model_->y1() - 0.01);
        glVertex2f(model_->x1() + 0.01, model_->y1() + 0.01);
        glVertex2f(model_->x1() - 0.01, model_->y1() + 0.01);
    glEnd();

    glFinish();
}

/*****************************************************************************
 * Main program
 ****************************************************************************/
GraphWidget *graph;
Fl_Text_Buffer *buffer;
Model *model = NULL;

void searchFunc(Fl_Widget *, void *);
void setModelFunc(Fl_Widget *, void *);
void timerFunc(void *);

void stopFunc();
void refreshFunc();

int main(int argc, const char* argv[]) {
    ScanModel scanModel;
    FibModel fibModel;
    GradModel gradModel;
    BounceModel bounceModel;
    BolzanoModel bolzanoModel;

    scanModel.init_search(XMin, XMax, Epsilon, &func);
    fibModel.init_search(XMin, XMax, Epsilon, &func);
    gradModel.init_search(XMin, XMax, Epsilon, &func);
    bounceModel.init_search(XMin, XMax, Epsilon, &func);
    bolzanoModel.init_search(XMin, XMax, Epsilon, &func);
    
    model = &scanModel;

    Fl_Double_Window *window = new Fl_Double_Window(700, 500, "One-dimensional Optimization Methods");
    window->resizable(window);

    // Controls setup begin
    window->begin();

    graph = new GraphWidget(10, 10, 480, 480, model, "Graph Widget");
    Fl_Button *btn = new Fl_Button(500, 10, 90, 25, "Search");
    btn->callback(searchFunc);

    // Output
    buffer = new Fl_Text_Buffer(1024);

    Fl_Text_Display *out = new Fl_Text_Display(500, 45, 190, 200);
    out->buffer(buffer);

    // Model chooser
    Fl_Group *modelChooser = new Fl_Group(500, 265, 190, 200, "Select model");
    modelChooser->box(FL_SHADOW_BOX);
    modelChooser->begin();

    Fl_Round_Button *btnScan = new Fl_Round_Button(510, 275, 170, 25, "Scanning");
    btnScan->type(FL_RADIO_BUTTON);
    btnScan->callback(setModelFunc, &scanModel);

    Fl_Round_Button *btnFib = new Fl_Round_Button(510, 310, 170, 25, "Fibonacci");
    btnFib->type(FL_RADIO_BUTTON);
    btnFib->callback(setModelFunc, &fibModel);

    Fl_Round_Button *btnGrad = new Fl_Round_Button(510, 345, 170, 25, "Gradient descent");
    btnGrad->type(FL_RADIO_BUTTON);
    btnGrad->callback(setModelFunc, &gradModel);

    Fl_Round_Button *btnBounce = new Fl_Round_Button(510, 380, 170, 25, "Heavy-ball method");
    btnBounce->type(FL_RADIO_BUTTON);
    btnBounce->callback(setModelFunc, &bounceModel);

    Fl_Round_Button *btnBolzano = new Fl_Round_Button(510, 415, 170, 25, "Bolzano");
    btnBolzano->type(FL_RADIO_BUTTON);
    btnBolzano->callback(setModelFunc, &bolzanoModel);

    btnScan->set();

    modelChooser->end();

    // Controls setup end
    window->end();
    window->show();

    int res = Fl::run();
    return res;
}

void searchFunc(Fl_Widget *w, void *) {
    if (model->search()) stopFunc();

    model->start();
    Fl::add_timeout(TimerInterval, timerFunc);
}

void timerFunc(void *) {
    model->step();
    refreshFunc();
    if (model->search())
        Fl::repeat_timeout(TimerInterval, timerFunc);
}

void setModelFunc(Fl_Widget *, void *o) {
    if (model->search()) stopFunc();
    model = (Model *)(o);
    TimerInterval = model->timer_interval();
    model->reset();
    graph->model(model);
    refreshFunc();
}

void stopFunc() {
    Fl::remove_timeout(timerFunc);
    model->stop();
}

void refreshFunc() {
    graph->redraw();

    buffer->select(0, buffer->length());
    buffer->remove_selection();

    char str[256];

    sprintf(str,
            "Func: exp(x)+1/x\n"
            "Epxilon: %13.6f\n"
            "Min X: %13.6f\n"
            "Min Y: %13.6f\n"
            "FuncCalcCount: %d\n"
            "%s\n",
        Epsilon,
        model->x1(), model->y1(),
        model->counter(),
        model->search() ? "Searching" : "Complete"
    );

    buffer->insert(0, str);
}
