
#include <FL/Fl.H>
#include <FL/gl.h>
#include <FL/glu.h>
#include <FL/glut.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>

#include <cstdio>
#include <cstdarg>
#include <cmath>

/*****************************************************************************
 * WaveModel
 *****************************************************************************/
struct Point {
    double x, z;
    double x0, z0;
};

static double surface_func_linear(double x, double delta, double epsilon) {
    return epsilon*(x/delta-0.5);
}

static double surface_func_sine(double x, double delta, double epsilon) {
    return epsilon * sin(2*M_PI*x/delta);
}

static double surface_func_cosine(double x, double delta, double epsilon) {
    return epsilon * cos(2*M_PI*x/delta);
}

static double surface_func_polygonal(double x, double delta, double epsilon) {
    return (x<delta*0.25)?(epsilon):((x>delta*0.75)?(-epsilon):(epsilon-4.0*epsilon*(x-delta*0.25)/(delta)));
}

class WaveModel {
public:
    double g;
    double delta, epsilon, h;

    double time, dtime;
    int frame;

    int xn, zn;
    int maxn;

    double *kn, *an, *sigman2, *sigman;
    Point **points;

    // double surface_func(double x);
    double (*surface_func)(double x, double delta, double epsilon);
    double furier_n(double d, int n);
    void calc_a();
    void calc_coeffs();
    double Gxn(double z0, double t, int n);
    double Gzn(double z0, double t, int n);
    double Fx(double x0, double z0, double t);
    double Fz(double x0, double z0, double t);

public:
    WaveModel();
    ~WaveModel();

    void reset();
    void step();
};

WaveModel::WaveModel() {
    xn = 30;
    zn = 15;
    maxn = 50;

    g = 9.81;
    delta = 1.0;
    epsilon = 0.1;
    h = 0.3;

    dtime = 0.01;

    surface_func = surface_func_linear;

    kn = new double[maxn];
    an = new double[maxn];
    sigman2 = new double[maxn];
    sigman = new double[maxn];

    points = new Point*[xn];
    for (int i=0; i<xn; i++)
        points[i] = new Point[zn];

    reset();
}

WaveModel::~WaveModel() {
    delete[] kn;
    delete[] an;
    delete[] sigman2;
    delete[] sigman;

    for (int i=0; i<xn; i++)
        delete[] points[i];
    delete[] points;
}

/* double WaveModel::surface_func(double x) {
	// return epsilon*(x/delta-0.5);
	return epsilon * sin(2*M_PI*x/delta);
	// return (x<delta*0.25)?(epsilon):((x>delta*0.75)?(-epsilon):(epsilon-4.0*epsilon*(x-delta*0.25)/(delta)));
} */

double WaveModel::furier_n(double d, int n) {
    const int MaxIter = 50000;
    double minx, maxx, dx;
    double *fi = new double[MaxIter+1];

    minx = 0.0;
    maxx = d;
    dx = (maxx - minx) / double(MaxIter);
    for (int i=0; i<MaxIter+1; i++) {
        double xi = minx + dx * i;
        fi[i] = surface_func(xi, delta, epsilon) * cos(kn[n-1] * xi);
    }

    int k = MaxIter >> 1;
    double s, s1, s2, s4;
    s1 = fi[0] + fi[MaxIter];
    s2 = s4 = 0.0;
    for (int i=2; i<=k; i++)
        s2 += fi[(i-1)*2-1];
    for (int i=1; i<=k; i++)
        s4 += fi[(i-1)*2];

    s = (s1 + 2.0 * s2 + 4.0 * s4) * dx / 3.0;

    delete[] fi;

    return 2.0 * s / d;
}

void WaveModel::calc_a() {
    for (int i=1; i<=maxn; i++) {
        // double dkn = delta * kn[i-1];
        // an[i-1] = epsilon * (sin(dkn)+2.0*(cos(dkn)-1.0)/dkn)/dkn;
        an[i-1] = furier_n(delta, i);
    }
}

void WaveModel::calc_coeffs() {
    for (int i=1; i<=maxn; i++) {
        kn[i-1] = M_PI*double(i)/delta;
        sigman2[i-1] = g*kn[i-1]*tanh(kn[i-1]*h);
        sigman[i-1] = sqrt(sigman2[i-1]);
    }

    calc_a();
}

double WaveModel::Gxn(double z0, double t, int n) {
    return -kn[n-1]*an[n-1]*cosh(kn[n-1]*(z0+h))*cos(sigman[n-1]*t)/(sigman2[n-1]*cosh(kn[n-1]*h));
}

double WaveModel::Gzn(double z0, double t, int n) {
    return -kn[n-1]*an[n-1]*sinh(kn[n-1]*(z0+h))*cos(sigman[n-1]*t)/(sigman2[n-1]*cosh(kn[n-1]*h));
}

double WaveModel::Fx(double x0, double z0, double t) {
    double sum = 0.0;
    for (int i=1; i<=maxn; i++) {
        sum += Gxn(z0, t, i)*sin(kn[i-1]*x0);
    }
    return g*sum;
}

double WaveModel::Fz(double x0, double z0, double t) {
    double sum = 0.0;
    for (int i=1; i<=maxn; i++) {
        sum += Gzn(z0, t, i)*cos(kn[i-1]*x0);
    }
    return g*sum;
}

void WaveModel::reset() {
    time = 0.0;
    frame = 0;

    calc_coeffs();

    double dx, dz;
    dx = delta / double(xn-1);
    dz = h / double(zn-1);

    for (int i=0; i<xn; i++)
        for (int j=0; j<zn; j++) {
            points[i][j].x0 = dx * double(i);
            points[i][j].z0 = -dz * double(j);
            points[i][j].x = points[i][j].x0 + Fx(points[i][j].x0, points[i][j].z0, time);
            points[i][j].z = points[i][j].z0 - Fz(points[i][j].x0, points[i][j].z0, time);
        }
}

void WaveModel::step() {
    frame++;
    time = double(frame) * dtime;
    for (int i=0; i<xn; i++)
        for (int j=0; j<zn; j++) {
            points[i][j].x = points[i][j].x0 + Fx(points[i][j].x0, points[i][j].z0, time);
            points[i][j].z = points[i][j].z0 - Fz(points[i][j].x0, points[i][j].z0, time);
        }
}

/*****************************************************************************
 * WaveWidget
 *****************************************************************************/
static const double XMin = -1.0;
static const double XMax = 1.0;
static const double YMin = -1.0;
static const double YMax = 0.5;

static const int TickCount = 20;
static const int TickSize = 15;
static const int Margin = 40;

typedef struct {
    float x, y;
} Point2d;

class WaveWidget: public Fl_Gl_Window {
    void draw();

    double pixelX_, pixelY_;
    double stepX_, stepY_;
    Point2d ticksX[(TickCount + 1) * 2];
    Point2d ticksY[(TickCount + 1) * 2];

public:
    WaveModel *model_;

    WaveWidget(int X, int Y, int W, int H, WaveModel *model = NULL, const char *l = 0);
};

static void PrintText(void *font, const char *fmt, ...) {
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

WaveWidget::WaveWidget(int X, int Y, int W, int H, WaveModel *model, const char *l)
    : Fl_Gl_Window(X, Y, W, H, l), model_(model) {
    pixelX_ = (XMax - XMin) / (this->w() - Margin*2 - TickSize);
    pixelY_ = (YMax - YMin) / (this->h() - Margin*2 - TickSize);

    // Y Ticks
    double dTick = (YMax - YMin) / TickCount;
    for (int i=0; i<=TickCount; i++) {
        double y = YMin + i * dTick;
        ticksY[i * 2].x = XMin;
        ticksY[i * 2].y = y;

        double tickScale = (i % 2) ? 0.5 : 1.0;
        ticksY[i * 2 + 1].x = XMin - TickSize * pixelX_ * tickScale;
        ticksY[i * 2 + 1].y = y;
    }

    // X Ticks
    dTick = (XMax - XMin) / TickCount;
    for (int i=0; i<=TickCount; i++) {
        double x = XMin + i * dTick;
        ticksX[i * 2].y = YMin;
        ticksX[i * 2].x = x;

        double tickScale = (i % 2) ? 0.5 : 1.0;
        ticksX[i * 2 + 1].y = YMin - TickSize * pixelY_ * tickScale;
        ticksX[i * 2 + 1].x = x;
    }
}

void WaveWidget::draw() {
    if (!this->valid()) {
        pixelX_ = (XMax - XMin) / (this->w() - Margin*2 - TickSize);
        pixelY_ = (YMax - YMin) / (this->h() - Margin*2 - TickSize);

        glViewport(0, 0, this->w(), this->h());
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(XMin - pixelX_*(Margin+TickSize),
                XMax + pixelX_*Margin,
                YMin - pixelY_*(Margin+TickSize),
                YMax + pixelY_*Margin,
                -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    if (!this->context_valid()) {
        glShadeModel(GL_SMOOTH);
        glClearColor(1.0, 1.0, 1.0, 0.5);
        glClearDepth(1.0);
        glDisable(GL_DEPTH_TEST);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glColor3f(0.75, 0.75, 0.75);

    glLineWidth(2);
    glBegin(GL_LINE_STRIP);
    glVertex3f( XMax, YMax, 0.0);
    glVertex3f( XMax, YMin, 0.0);
    glVertex3f( XMin, YMin, 0.0);
    glVertex3f( XMin, YMax, 0.0);
    glEnd();
    
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex3f( XMin, 0.0, 0.0);
    glVertex3f( XMax, 0.0, 0.0);
    glEnd();

    if (!model_) return;

    double scalex = (XMax - XMin) / model_->delta;
    double scalez = (/*YMax*/ - YMin) / model_->h;

    // glColor3f(0.5, 0.75, 1.0);
    glColor3f(0.0, 0.0, 0.0);
    glPointSize(3.0);
    glBegin(GL_POINTS);
    for (int i=0; i<model_->xn; i++)
        for (int j=0; j<model_->zn; j++) {
            glVertex3f(model_->points[i][j].x * scalex + XMin,
                       ((model_->h + model_->points[i][j].z) * scalez) + YMin, 0.0);
        }
    glEnd();

    glLineWidth(1.0);
    for (int j=0; j<model_->zn; j++) {
        glBegin(GL_LINE_STRIP);
        for (int i=0; i<model_->xn-1; i++) {
            glVertex3f(model_->points[i][j].x * scalex + XMin,
                       ((model_->h + model_->points[i][j].z) * scalez) + YMin, 0.0);
            glVertex3f(model_->points[i+1][j].x * scalex + XMin,
                       ((model_->h + model_->points[i+1][j].z) * scalez) + YMin, 0.0);
        }
        glEnd();
    }

    glRasterPos2d(XMin, YMax);
    PrintText(GLUT_BITMAP_HELVETICA_12, "time : %.4f", model_->time);

    glColor3f(0.25, 0.25, 0.25);

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
    /* glRasterPos2d(XMin,
                  YMin - pixelY_ * (Margin / 2 + TickSize));
    PrintText(GLUT_BITMAP_HELVETICA_10, "%.1f", 0.0);

    glRasterPos2d(XMax,
                  YMin - pixelY_ * (Margin / 2 + TickSize));
    PrintText(GLUT_BITMAP_HELVETICA_10, "%.1f", model_->delta);

    glRasterPos2d(XMin - pixelX_ * (Margin / 2 + TickSize),
                  0.0 - pixelY_ * (Margin / 2));
    PrintText(GLUT_BITMAP_HELVETICA_10, "%.1f", 0.0);

    glRasterPos2d(XMin - pixelX_ * (Margin / 2 + TickSize),
                  YMin + pixelY_ * (Margin / 2));
    PrintText(GLUT_BITMAP_HELVETICA_10, "%.1f", (-model_->h)); */

    // Axis
    int len;
    static char buffer[15];
    void *font = GLUT_BITMAP_HELVETICA_10;

    // X-Axis labels
    sprintf(buffer, "%.1f", 0.0);
    glRasterPos2d(XMin, YMin-pixelY_*(Margin*0.5+TickSize));
    PrintText(font, buffer);

    sprintf(buffer, "%.1f", model_->delta);
    len = glutBitmapLength(font, (const unsigned char *)buffer);
    glRasterPos2d(XMax-pixelX_*len, YMin-pixelY_*(Margin*0.5+TickSize));
    PrintText(font, buffer);

    // Y-Axis labels
    sprintf(buffer, "%.1f", YMax);
    len = glutBitmapLength(font, (const unsigned char *)buffer);
    glRasterPos2d(XMin-pixelX_*(len+TickSize), YMax);
    PrintText(font, buffer);

    sprintf(buffer, "%.1f", 0.0);
    len = glutBitmapLength(font, (const unsigned char *)buffer);
    glRasterPos2d(XMin-pixelX_*(len+TickSize), 0.0);
    PrintText(font, buffer);

    sprintf(buffer, "%.1f", -model_->h);
    len = glutBitmapLength(font, (const unsigned char *)buffer);
    glRasterPos2d(XMin-pixelX_*(len+TickSize), YMin);
    PrintText(font, buffer);

    glFinish();
}

/*****************************************************************************
 * WaveWindow class
 *****************************************************************************/
class WaveWindow: public Fl_Window {
    WaveModel model;
    WaveWidget *ww;
    Fl_Choice *surface_choice;
    Fl_Input *g_in, *h_in, *delta_in, *eps_in, *dtime_in;
    Fl_Button *restart_btn, *step_btn, *start_btn, *stop_btn;

    void get_inputs();
    void set_inputs();

    bool animating;

    static void restart_cb(Fl_Widget *w, void *v);
    static void step_cb(Fl_Widget *w, void *v);
    static void timer_cb(void *v);
    static void start_cb(Fl_Widget *w, void *v);
    static void stop_cb(Fl_Widget *w, void *v);

public:
    WaveWindow(int W, int H, const char *l = NULL);
    virtual ~WaveWindow();
};

/*****************************************************************************
 * WaveWindow methods
 *****************************************************************************/
const double TimerInterval = 0.01;

WaveWindow::WaveWindow(int W, int H, const char *l)
    : Fl_Window(W, H, l) {
    animating = false;

    ww = new WaveWidget(10, 10, 480, 480, &model);

    surface_choice = new Fl_Choice(570,  10, 90, 25, "Surface :");
    surface_choice->add("Linear");
    surface_choice->add("Sine");
    surface_choice->add("Cosine");
    surface_choice->add("Polygonal");
    surface_choice->value(0);

    g_in     = new Fl_Input(570,  40, 90, 25, "g =");
    h_in     = new Fl_Input(570,  70, 90, 25, "h =");
    delta_in = new Fl_Input(570, 100, 90, 25, "delta =");
    eps_in   = new Fl_Input(570, 130, 90, 25, "eps =");
    dtime_in = new Fl_Input(570, 160, 90, 25, "dtime =");

    set_inputs();

    restart_btn = new Fl_Button(570, 200, 90, 25, "Restart");
    step_btn    = new Fl_Button(570, 230, 90, 25, "Step");
    start_btn   = new Fl_Button(570, 260, 90, 25, "Start");
    stop_btn    = new Fl_Button(570, 290, 90, 25, "Stop");
    restart_btn->callback(restart_cb, this);
    step_btn->callback(step_cb, this);
    start_btn->callback(start_cb, this);
    stop_btn->callback(stop_cb, this);
    stop_btn->deactivate();

    this->end();
}

WaveWindow::~WaveWindow() {
    //
}

void WaveWindow::set_inputs() {
    static char str[30];

    sprintf(str, "%.4f", model.g);
    g_in->value(str);
    sprintf(str, "%.4f", model.h);
    h_in->value(str);
    sprintf(str, "%.4f", model.delta);
    delta_in->value(str);
    sprintf(str, "%.4f", model.epsilon);
    eps_in->value(str);
    sprintf(str, "%.4f", model.dtime);
    dtime_in->value(str);
}

void WaveWindow::get_inputs() {
    static char str[30];

    strcpy(str, g_in->value());
    model.g = atof(str);
    strcpy(str, h_in->value());
    model.h = atof(str);
    strcpy(str, delta_in->value());
    model.delta = atof(str);
    strcpy(str, eps_in->value());
    model.epsilon = atof(str);
    strcpy(str, dtime_in->value());
    model.dtime = atof(str);

    int v = surface_choice->value();
    switch (v) {
    case 0:
        model.surface_func = surface_func_linear;
        break;

    case 1:
        model.surface_func = surface_func_sine;
        break;

    case 2:
        model.surface_func = surface_func_cosine;
        break;

    case 3:
        model.surface_func = surface_func_polygonal;
        break;
    }
}

void WaveWindow::restart_cb(Fl_Widget * /*w*/, void *v) {
    WaveWindow *wnd = reinterpret_cast<WaveWindow *>(v);
    wnd->get_inputs();
    wnd->model.reset();
    wnd->ww->redraw();
}

void WaveWindow::step_cb(Fl_Widget * /*w*/, void *v) {
    WaveWindow *wnd = reinterpret_cast<WaveWindow *>(v);
    wnd->model.step();
    wnd->ww->redraw();
}

void WaveWindow::timer_cb(void * v) {
    WaveWindow *wnd = reinterpret_cast<WaveWindow *>(v);
    wnd->model.step();
    wnd->ww->redraw();
    Fl::repeat_timeout(TimerInterval, timer_cb, v);
}

void WaveWindow::start_cb(Fl_Widget * /*w*/, void *v) {
    WaveWindow *wnd = reinterpret_cast<WaveWindow *>(v);
    if (wnd->animating) return;
    wnd->animating = true;

    wnd->restart_btn->deactivate();
    wnd->step_btn->deactivate();
    wnd->start_btn->deactivate();
    wnd->stop_btn->activate();

    Fl::add_timeout(TimerInterval, timer_cb, v);
}

void WaveWindow::stop_cb(Fl_Widget * /*w*/, void *v) {
    WaveWindow *wnd = reinterpret_cast<WaveWindow *>(v);
    if (!wnd->animating) return;
    wnd->animating = false;

    wnd->restart_btn->activate();
    wnd->step_btn->activate();
    wnd->start_btn->activate();
    wnd->stop_btn->deactivate();

    Fl::remove_timeout(timer_cb);
}

/*****************************************************************************
 * main
 *****************************************************************************/
int main (int argc, char ** argv) {
    WaveWindow window(700, 500, "Wave Viewer");
    window.resizable(&window);
    window.show(argc, argv);
    return(Fl::run());
}
