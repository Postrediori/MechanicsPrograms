/*
 * MediaWave
 */

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/gl.h>
#include <FL/glu.h>
#include <FL/glut.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Tree.H>

#include <cstdio>
#include <cstdarg>

#include "pixmaps.h"

/*
 * Math routines
 */
int simq2(float a[2][2], float b[], float x[]) {
    float delta = a[0][0]*a[1][1]-a[1][0]*a[0][1];
    if (delta==0.0) return 1;
    x[0]=(b[0]*a[1][1]-b[1]*a[0][1])/delta;
    x[1]=(b[1]*a[0][0]-b[0]*a[1][0])/delta;
    return 0;
}

bool betweenf(float a, float x, float b) {
	return ((a<x) && (x<b));
}

/*
 * Haze model
 */

// Initial conditions
static const float DefL = 10.0;
static const int DefN = 100;
static const float DefA = 1.0;
static const float DefRho = 1.0;
static const float DefSigma = 0.1;

#define INIT_COND_CLOSED 0.0,1.0
#define INIT_COND_OPENED 1.0,0.0
#define INIT_COND(b,c) b,c

static const int DefUNType = 0;
static const int DefPNType = 0;

// u(x) initial conditions
const float UN_0 = 0.0;
const float UN_1 = 1.0;

// p(x) initial conditions
const float PN_0 = 0.0;
const float PN_1 = 1.0;

class HazeModel {
public:
    int N;
    float L;
    float sigma, a;
    float h, h2;
    float rho, rho_a;
    float t, tau, tau_h;

    int k;

    float bl, cl;
    float br, cr;

	float init_left, init_right;
	int un_type, pn_type;

    float un(float);
	float un_lin(float);
	float un1(float);
	float un2(float);
	float un3(float);
	float un4(float);
	float un5(float);
	float un6(float);

    float pn(float);
    float pn_lin(float);
    float pn1(float);
    float pn2(float);
    float pn3(float);
    float pn4(float);


    float *x, *x2;
    float *u1, *u2, *p1, *p2;
    float *U, *P;

public:
    HazeModel(float, int);
    ~HazeModel();

    void Init();
    void Reset();
    void Step();

    void InitFunc(int, int);
    void InitCondL(float, float);
    void InitCondR(float, float);
};

HazeModel::HazeModel(float l, int n)
 : N(n), L(l),
   x(NULL), x2(NULL),
   u1(NULL), u2(NULL), p1(NULL), p2(NULL),
   U(NULL), P(NULL) {
    InitFunc(DefUNType, DefPNType);
    InitCondL(INIT_COND_CLOSED);
    InitCondR(INIT_COND_CLOSED);

    Init();
}

HazeModel::~HazeModel() {
    if (x) delete[] x;
    if (x2) delete[] x2;
    if (u1) delete[] u1;
    if (u2) delete[] u2;
    if (p1) delete[] p1;
    if (p2) delete[] p2;
    if (U) delete[] U;
    if (P) delete[] P;
}

void HazeModel::InitFunc(int un_type, int pn_type) {
    this->un_type = un_type;
    this->pn_type = pn_type;
}

void HazeModel::InitCondL(float b, float c) {
    this->bl = b;
    this->cl = c;
}

void HazeModel::InitCondR(float b, float c) {
    this->br = b;
    this->cr = c;
}

void HazeModel::Init() {
    sigma = DefSigma;

    a = DefA;
    rho = DefRho;

    Reset();
}

void HazeModel::Reset() {
    h = L / N;
    h2 = h / 2;
    tau = sigma * h / a;
    rho_a = rho * a;
    tau_h = tau / h;

	init_left = L / 3.0;
	init_right = L * 2.0 / 3.0;

    if (x) delete[] x;
    if (x2) delete[] x2;
    if (u1) delete[] u1;
    if (u2) delete[] u2;
    if (p1) delete[] p1;
    if (p2) delete[] p2;
    if (U) delete[] U;
    if (P) delete[] P;

    x = new float[N+1];
    x2 = new float[N];

    for (int i=0; i<=N; i++) {
        x[i] = h * i;
    }
    for (int i=0; i<N; i++) {
        x2[i] = x[i] + h2;
    }

    u1 = new float[N];
    u2 = new float[N];
    p1 = new float[N];
    p2 = new float[N];

    U = new float [N+1];
    P = new float [N+1];

    // Initial u and p values
    for (int i=0; i<N; i++) {
        u1[i] = un(x2[i]);
        p1[i] = pn(x2[i]);
    }

    k = 0;
}

void HazeModel::Step() {
    k++;
    t = tau * k;

    float S[2][2];
    float Y[2], W[2];

    S[0][0] = 1.0;
    S[0][1] = -1.0/rho_a;
    S[1][0] = cl;
    S[1][1] = bl;
    Y[0]    = u1[0]-p1[0]/rho_a;
    Y[1]    = 0.0;
    simq2(S, Y, W);
    U[0] = W[0];
    P[0] = W[1];
    /* U[0] = bl * (-p1[0] + rho_a * u1[0]) / (cl + rho_a * bl);
    P[0] = cl * ( p1[0] - rho_a * u1[0]) / (cl + rho_a * bl); */

    S[0][0] = 1.0;
    S[0][1] = 1.0/rho_a;
    S[1][0] = cr;
    S[1][1] = br;
    Y[0]    = u1[N-1]+p1[N-1]/rho_a;
    Y[1]    = 0.0;
    simq2(S, Y, W);
    U[N] = W[0];
    P[N] = W[1];
    /* U[N] = br * ( p1[N-1] + rho_a * u1[N-1]) / (-cl + rho_a * br);
    P[N] = cr * ( p1[N-1] + rho_a * u1[N-1]) / ( cl - rho_a * br); */

    for (int i=1; i<N; i++) {
        U[i] = ((u1[i]+u1[i-1])-(p1[i]-p1[i-1])/rho_a)/2.0;
        P[i] = ((p1[i]+p1[i-1])-(u1[i]-u1[i-1])*rho_a)/2.0;
    }

    for (int i=0; i<N; i++) {
        u2[i] = u1[i]-(P[i+1]-P[i])*tau_h/rho;
        p2[i] = p1[i]-(U[i+1]-U[i])*rho_a*tau_h*a;
    }

    for (int i=0; i<N; i++) {
        u1[i] = u2[i];
        p1[i] = p2[i];
    }
}

float HazeModel::un(float x) {
	switch (un_type) {
	case 0: return un_lin(x);
	case 1: return un1(x);
	case 2: return un2(x);
	case 3: return un3(x);
	case 4: return un4(x);
	case 5: return un5(x);
	case 6: return un6(x);
    default: return un_lin(x);
	}
}

float HazeModel::un_lin(float /*x*/) {
    return UN_0;
}

float HazeModel::un1(float x) {
    if (x>init_left && x<init_right) return UN_0;
    else return UN_1;
}

float HazeModel::un2(float x) {
    if (x<init_left) return UN_1;
    else return UN_0;
}

float HazeModel::un3(float x) {
    if (x>init_left && x<init_right) return UN_1;
    else return UN_0;
}

float HazeModel::un4(float x) {
    if (x>init_right) return UN_1;
    else return UN_0;
}

float HazeModel::un5(float x) {
    if (x<L/2) return -UN_1;
    else return UN_1;
}

float HazeModel::un6(float x) {
    if (x<L/2) return UN_1;
    else return -UN_1;
}

float HazeModel::pn(float x) {
	switch (pn_type) {
	case 0: return pn_lin(x);
	case 1: return pn1(x);
	case 2: return pn2(x);
	case 3: return pn3(x);
	case 4: return pn4(x);
    default: return pn_lin(x);
	}
}

float HazeModel::pn_lin(float /*x*/) {
    return PN_0;
}

float HazeModel::pn1(float x) {
    if (x>init_left && x<init_right) return PN_0;
    else return PN_1;
}

float HazeModel::pn2(float x) {
    if (x<init_left) return PN_1;
    else return PN_0;
}

float HazeModel::pn3(float x) {
    if (x>init_left && x<init_right) return PN_1;
    else return PN_0;
}

float HazeModel::pn4(float x) {
    if (x>init_right) return PN_1;
    else return PN_0;
}

/*
 * Function plot widget
 */
struct Point2D {
    float x, y;
};

static const float DefXMin =-1.0;
static const float DefXMax = 1.0;
static const float DefYMin =-1.0;
static const float DefYMax = 1.0;

static const float DefXAxis = 0.0;
static const float DefYAxis = 0.0;

static const int DefMargin = 25;
static const int DefTickSize = 10;
static const int DefTickCount = 20;

class PlotWidget: public Fl_Gl_Window {
private:
    float xmin_, xmax_, ymin_, ymax_;
	float xaxis_, yaxis_;
    int tick_count_, tick_size_, margin_;

    float pixel_x, pixel_y;
    Point2D *ticks_x;
    Point2D *ticks_y;

    int point_count_;
    Point2D *points;

    float tick_color[3];
    float axis_color[3];
    float plot_color[3];
    float text_color[3];

    void pixel_scale();

    void draw_box();
    void draw_ticks();
    void draw_axis();
    void draw_heatmap();
    void draw_plot();
    void draw_legend();

    virtual void draw();

public:
    PlotWidget(int X, int Y, int W, int H, const char *);
    ~PlotWidget();

    void print_text(void *, float, float, unsigned char, const char *, ...);

	void axis(float, float);
    void view_range(float, float, float, float);
    void ticks(int, int);
    void margin(int);
    void plot(int, float*, float*);
};

PlotWidget::PlotWidget(int X, int Y, int W, int H, const char *lbl = NULL)
 : Fl_Gl_Window(X, Y, W, H, lbl),
   ticks_x(NULL), ticks_y(NULL), points(NULL) {
    tick_color[0] = 0.0; tick_color[1] = 0.0; tick_color[2] = 0.0;
    axis_color[0] = 0.75; axis_color[1] = 0.75; axis_color[2] = 0.75;
    plot_color[0] = 1.0; plot_color[1] = 0.0; plot_color[2] = 0.0;
    text_color[0] = 0.25; text_color[1] = 0.25; text_color[2] = 0.25;

	this->axis(DefXAxis, DefYAxis);
    this->view_range(DefXMin, DefXMax, DefYMin, DefYMax);
    this->margin(DefMargin);
    this->ticks(DefTickCount, DefTickSize);
}

PlotWidget::~PlotWidget() {
    if (ticks_x) delete[] ticks_x;
    if (ticks_y) delete[] ticks_y;
    if (points) delete[] points;
}

void PlotWidget::pixel_scale() {
    pixel_x = (xmax_ - xmin_) / (this->w() - margin_ * 2 - tick_size_);
    pixel_y = (ymax_ - ymin_) / (this->h() - margin_ * 2 - tick_size_);
}

void PlotWidget::axis(float xaxis, float yaxis) {
	this->xaxis_ = xaxis;
	this->yaxis_ = yaxis;
}

void PlotWidget::view_range(float xmin, float xmax, float ymin, float ymax) {
    this->xmin_ = xmin;
    this->xmax_ = xmax;
    this->ymin_ = ymin;
    this->ymax_ = ymax;
}

void PlotWidget::ticks(int count, int size) {
    this->tick_count_ = count;
    this->tick_size_ = size;

    if (ticks_x) delete[] ticks_x;
    if (ticks_y) delete[] ticks_y;

    ticks_x = new Point2D[(tick_count_ + 1) * 2];
    ticks_y = new Point2D[(tick_count_ + 1) * 2];

    pixel_scale();

    // Calculate x&y tick positions
    float dtick;
    float x, y, tick_scale;

    // Y ticks
    dtick = (ymax_ - ymin_) / tick_count_;
    for (int i=0; i<=tick_count_; i++) {
        y = ymin_ + i * dtick;

        ticks_y[i * 2].x = xmin_;
        ticks_y[i * 2].y = y;

        tick_scale = (i % 2) ? 0.5 : 1.0;
        ticks_y[i * 2 + 1].x = xmin_ - tick_size_ * pixel_x * tick_scale;
        ticks_y[i * 2 + 1].y = y;
    }

    // X ticks
    dtick = (xmax_ - xmin_) / tick_count_;
    for (int i=0; i<=tick_count_; i++) {
        x = xmin_ + i * dtick;

        ticks_x[i * 2].x = x;
        ticks_x[i * 2].y = ymin_;

        tick_scale = (i % 2) ? 0.5 : 1.0; // 1.0
        ticks_x[i * 2 + 1].x = x;
        ticks_x[i * 2 + 1].y = ymin_ - tick_size_ * pixel_y * tick_scale;
    }
}

void PlotWidget::margin(int m) {
    this->margin_ = m;
}

const int ALIGN_TOP = 0x01;
const int ALIGN_MIDDLE = 0x02;
const int ALIGN_BOTTOM = 0x04;
const int ALIGN_LEFT = 0x10;
const int ALIGN_CENTER = 0x20;
const int ALIGN_RIGHT = 0x40;
void PlotWidget::print_text(void *font, float x, float y, unsigned char align, const char *fmt, ...) {
    char text[256];
    va_list ap;

    if (fmt==NULL) return;

    va_start(ap, fmt);
    vsnprintf(text, 256, fmt, ap);
    va_end(ap);

    int w, h;
    h = glutBitmapHeight(font);

    w = 0;
    char *p = (char *) text;
    while (*p!='\0') {
        w += glutBitmapWidth(font, *p);
        p++;
    }

    float tx, ty, tw, th;
    tx = x;
    ty = y;
    tw = w * pixel_x;
    th = h * pixel_y;

    if (align==0) align=ALIGN_RIGHT | ALIGN_TOP;

    if (align&ALIGN_LEFT) tx -= tw;
    else if (align&ALIGN_CENTER) tx -= tw/2;

    if (align&ALIGN_BOTTOM) ty -= th;
    else if (align&ALIGN_MIDDLE) ty -= th/2;

    glRasterPos2d(tx, ty);
    glutBitmapString(font, (const unsigned char *)text);
}

void PlotWidget::draw_box() {
    glColor3fv(tick_color);

    glBegin(GL_LINE_LOOP);
    glVertex2f(xmin_, ymin_);
    glVertex2f(xmax_, ymin_);
    glVertex2f(xmax_, ymax_);
    glVertex2f(xmin_, ymax_);
    glEnd();
}

void PlotWidget::draw_ticks() {
    glColor3fv(tick_color);

    glBegin(GL_LINES);
    if (ticks_x) {
        for (int i=0; i<=tick_count_; i++) {
            glVertex2f(ticks_x[i*2  ].x, ticks_x[i*2  ].y);
            glVertex2f(ticks_x[i*2+1].x, ticks_x[i*2+1].y);
        }
    }

    if (ticks_y) {
        for (int i=0; i<=tick_count_; i++) {
            glVertex2f(ticks_y[i*2  ].x, ticks_y[i*2  ].y);
            glVertex2f(ticks_y[i*2+1].x, ticks_y[i*2+1].y);
        }
    }
    glEnd();
}

void PlotWidget::draw_axis() {
    glColor3fv(axis_color);

    // X axis
    if (betweenf(ymin_, xaxis_, ymax_)) {
        glBegin(GL_LINES);
        glVertex2f(xmin_, xaxis_);
        glVertex2f(xmax_, xaxis_);
        glEnd();
    }

    // Y axis
    if (betweenf(xmin_, yaxis_, xmax_)) {
        glBegin(GL_LINES);
        glVertex2f(yaxis_, xmin_);
        glVertex2f(yaxis_, xmax_);
        glEnd();
    }
}

void PlotWidget::draw_heatmap() {
    static const size_t PaletteSize = 11;
    static const GLubyte Palette[PaletteSize][4] = {
        {0xa5, 0x00, 0x27, 0x00},
        {0xd7, 0x30, 0x27, 0x00},
        {0xf4, 0x6d, 0x43, 0x00},
        {0xfd, 0xae, 0x61, 0x00},
        {0xfe, 0xe0, 0x90, 0x00},
        {0xff, 0xff, 0xbf, 0x00},
        {0xe0, 0xf3, 0xf9, 0x00},
        {0xab, 0xd9, 0xea, 0x00},
        {0x74, 0xad, 0xd1, 0x00},
        {0x45, 0x75, 0xb4, 0x00},
        {0x31, 0x36, 0x95, 0x00},
    };

    glBegin(GL_QUADS);
    if (points) {
        for (int i=0; i<point_count_; i++) {
            double y = points[i*2].y;
            double t = 1. - (y - ymin_) / (ymax_ - ymin_);
            size_t k = static_cast<size_t>((static_cast<double>(PaletteSize) * t)) % PaletteSize;
            glColor4ubv(Palette[k]);

            glVertex2f(points[i*2].x, ymin_);
            glVertex2f(points[i*2+1].x, ymin_);
            glVertex2f(points[i*2+1].x, ymax_);
            glVertex2f(points[i*2].x, ymax_);
        }
    }
    glEnd();

}

void PlotWidget::draw_plot() {
    glColor3fv(plot_color);
    glBegin(GL_LINES);
    if (points) {
        for (int i=0; i<point_count_; i++) {
            glVertex2f(points[i*2  ].x, points[i*2  ].y);
            glVertex2f(points[i*2+1].x, points[i*2+1].y);
        }
    }
    glEnd();
}

#define LEGEND_FONT GLUT_BITMAP_HELVETICA_10
void PlotWidget::draw_legend() {
    glColor3fv(text_color);

    print_text(LEGEND_FONT, (xmax_-xmin_)/2.0, ymax_+margin_*pixel_y/2.0,
               ALIGN_CENTER | ALIGN_MIDDLE, this->label());

    print_text(LEGEND_FONT, -tick_size_*pixel_x, 0.0,
               ALIGN_LEFT | ALIGN_MIDDLE, "%.1f", 0.0);

    print_text(LEGEND_FONT, xmin_-tick_size_*pixel_x, ymin_,
               ALIGN_LEFT | ALIGN_MIDDLE, "%.1f", ymin_);

    print_text(LEGEND_FONT, xmin_-tick_size_*pixel_x, ymax_,
               ALIGN_LEFT | ALIGN_MIDDLE, "%.1f", ymax_);

    print_text(LEGEND_FONT, xmin_, ymin_-tick_size_*pixel_y,
               ALIGN_CENTER | ALIGN_BOTTOM, "%.1f", xmin_);
    print_text(LEGEND_FONT, xmax_, ymin_-tick_size_*pixel_y,
               ALIGN_CENTER | ALIGN_BOTTOM, "%.1f", xmax_);
}

void PlotWidget::draw() {
    if (!this->valid()) {
        pixel_scale();
        glViewport(0, 0, this->w(), this->h());
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(xmin_ - pixel_x * (margin_ + tick_size_),
                   xmax_ + pixel_x * margin_,
                   ymin_ - pixel_y * (margin_ + tick_size_),
                   ymax_ + pixel_y * margin_);
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

    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    draw_heatmap();

    // Draw ticks and bounding box
    this->draw_box();
    this->draw_ticks();

    this->draw_axis();

    this->draw_legend();

    this->draw_plot();

    glFinish();
}

void PlotWidget::plot(int count, float *x, float *y) {
    this->point_count_ = count;

    if (points) delete[] points;
    points = new Point2D[(this->point_count_+1)*2];

    for (int i=0; i<this->point_count_; i++) {
        points[i*2  ].x = x[i];
        points[i*2  ].y = y[i];

        points[i*2+1].x = x[i+1];
        points[i*2+1].y = y[i  ];
    }
}

/*
 * Settings
 */
#define SIDE_LEFT  (void *)1
#define SIDE_RIGHT (void *)2

#define SIDE_CLOSE  (void *)1
#define SIDE_OPEN   (void *)2
#define SIDE_MANUAL (void *)3

struct SettingsHelper {
    float L;
    int N;

    float a;
    float rho;
    float sigma;

    int un_type;
    int pn_type;

    void *left_side_type;
    void *right_side_type;

    float bl, cl;
    float br, cr;
};

/*
 * Main Window
 */
static const double TimerInterval = 0.01;

class MainWindow: public Fl_Window {
private:
    HazeModel *model_;

    PlotWidget *uw, *pw;
    Fl_Button *step_btn, *start_btn, *stop_btn, *settings_btn;

    void redraw();

    static void step_cb_st(Fl_Widget *, void *);
    void step_cb();

    static void start_cb_st(Fl_Widget *, void *);
    void start_cb();

    static void stop_cb_st(Fl_Widget *, void *);
    void stop_cb();

    static void settings_cb_st(Fl_Widget *, void *);
    void settings_cb();

    static void timer_cb_st(void *);
    void timer_cb();

    // Settings window properties
    SettingsHelper helper_;
    Fl_Window *settings_wnd;

    Fl_Button *close_btn, *apply_btn;

    Fl_Input *len_in, *n_in;
    Fl_Input *a_in, *rho_in, *sigma_in;
    Fl_Choice *u_choice, *p_choice;
    Fl_Tree *left_side_choice, *right_side_choice;
    Fl_Input *bl_in, *cl_in, *br_in, *cr_in;

	static void create_side_selector(Fl_Tree *);
	static void set_tree_icons(Fl_Tree *);
	static void select_side_type(Fl_Tree *, void *);
    void create_settings_wnd();

    static void close_cb_st(Fl_Widget *, void *);
    void close_cb();

    static void apply_cb_st(Fl_Widget *, void *);
    void apply_cb();

    static void left_side_cb_st(Fl_Widget *, void *);
    void left_side_cb();

    static void right_side_cb_st(Fl_Widget *, void *);
    void right_side_cb();

    void set_inputs(SettingsHelper);
    SettingsHelper get_inputs();

    void set_lcond_inputs(SettingsHelper helper);
    void get_lcond_inputs(SettingsHelper &helper);

    void set_rcond_inputs(SettingsHelper helper);
    void get_rcond_inputs(SettingsHelper &helper);

public:
    MainWindow(HazeModel *);
    ~MainWindow();

    void helper(SettingsHelper);
    SettingsHelper helper();
};

MainWindow::MainWindow(HazeModel *model)
 : Fl_Window(700, 500, "MediaWave"), model_(model) {
    create_settings_wnd();

    this->resizable(this);
    this->begin();

    uw = new PlotWidget(10, 10, 480, 235, "u(x)");
    pw = new PlotWidget(10, 255, 480, 235, "p(x)");

    settings_btn = new Fl_Button(550, 20, 90, 25, "Settings");
    step_btn    = new Fl_Button(550, 50, 90, 25, "Step");
    start_btn   = new Fl_Button(550, 80, 90, 25, "Start");
    stop_btn    = new Fl_Button(550, 110, 90, 25, "Stop");

    this->end();

    step_btn->callback(step_cb_st, this);
    start_btn->callback(start_cb_st, this);
    stop_btn->callback(stop_cb_st, this);
    settings_btn->callback(settings_cb_st, this);

    stop_btn->deactivate();

    uw->view_range(0.0, DefL, -2.0, 2.0);
    uw->ticks(DefTickCount, DefTickSize);

    pw->view_range(0.0, DefL, -2.0, 2.0);
    pw->ticks(DefTickCount, DefTickSize);

    uw->plot(DefN, model->x, model->u1);
    pw->plot(DefN, model->x, model->p1);

    helper_.L = DefL;
    helper_.N = DefN;
    helper_.a = DefA;
    helper_.rho = DefRho;
    helper_.sigma = DefSigma;
    helper_.un_type = DefUNType;
    helper_.pn_type = DefPNType;
    helper_.left_side_type = SIDE_CLOSE;
    helper_.right_side_type = SIDE_CLOSE;
    helper_.bl = 1.0;
    helper_.cl = 0.0;
    helper_.br = 1.0;
    helper_.cr = 0.0;

    helper(helper_);
}

MainWindow::~MainWindow() {
    //
}

void MainWindow::redraw() {
    uw->plot(model_->N, model_->x, model_->u1);
    pw->plot(model_->N, model_->x, model_->p1);

    uw->redraw();
    pw->redraw();
}

void MainWindow::helper(SettingsHelper h) {
	void *side;

    helper_ = h;

    model_->L = helper_.L;
    model_->N = helper_.N;

    model_->a = helper_.a;
    model_->rho = helper_.rho;
    model_->sigma = helper_.sigma;

    model_->InitFunc(helper_.un_type, helper_.pn_type);

    side = helper_.left_side_type;
    if (side==SIDE_CLOSE) model_->InitCondL(INIT_COND_CLOSED);
    else if (side==SIDE_OPEN)  model_->InitCondL(INIT_COND_OPENED);
    else if (side==SIDE_MANUAL) model_->InitCondL(helper_.bl, helper_.cl);

    side = helper_.right_side_type;
    if (side==SIDE_CLOSE) model_->InitCondR(INIT_COND_CLOSED);
    else if (side==SIDE_OPEN)  model_->InitCondR(INIT_COND_OPENED);
    else if (side==SIDE_MANUAL) model_->InitCondR(helper_.br, helper_.cr);

    uw->view_range(0.0, model_->L, -2.0, 2.0);
    uw->ticks(DefTickCount, DefTickSize);
    uw->invalidate();

    pw->view_range(0.0, model_->L, -2.0, 2.0);
    pw->ticks(DefTickCount, DefTickSize);
    pw->invalidate();

    this->model_->Reset();
    this->redraw();
}

SettingsHelper MainWindow::helper() {
    return this->helper_;
}

void MainWindow::step_cb_st(Fl_Widget *, void *v) {
    MainWindow *w = (MainWindow *)v;
    w->step_cb();
}

void MainWindow::step_cb() {
    this->model_->Step();
    this->redraw();
}

void MainWindow::start_cb_st(Fl_Widget *, void *v) {
    MainWindow *w = (MainWindow *)v;
    w->start_cb();
}

void MainWindow::start_cb() {
    this->settings_btn->deactivate();
    this->step_btn->deactivate();
    this->start_btn->deactivate();
    this->stop_btn->activate();

    Fl::add_timeout(TimerInterval, timer_cb_st, this);
}

void MainWindow::stop_cb_st(Fl_Widget *, void *v) {
    MainWindow *w = (MainWindow *)v;
    w->stop_cb();
}

void MainWindow::stop_cb() {
    this->settings_btn->activate();
    this->step_btn->activate();
    this->start_btn->activate();
    this->stop_btn->deactivate();

    Fl::remove_timeout(timer_cb_st, this);
}

void MainWindow::settings_cb_st(Fl_Widget *, void *v) {
    MainWindow *w = (MainWindow *)v;
    w->settings_cb();
}

void MainWindow::settings_cb() {
    this->set_inputs(helper_);
    settings_wnd->show();
}

void MainWindow::timer_cb_st(void *v) {
    MainWindow *w = (MainWindow *)v;
    w->timer_cb();
}

void MainWindow::timer_cb() {
    this->model_->Step();
    this->redraw();

    Fl::repeat_timeout(TimerInterval, timer_cb_st, this);
}

// Settings window
const char label_close_left[]  = "[==";
const char label_close_right[] = "==]";
const char label_open[]        = "===";
const char label_manual[]      = "~~~";

void MainWindow::create_side_selector(Fl_Tree *t) {
	t->showroot(0);
	t->connectorstyle(FL_TREE_CONNECTOR_NONE);
	t->selectmode(FL_TREE_SELECT_SINGLE);
	t->connectorwidth(0);
	t->marginleft(0);
	t->visible_focus(0);
}

void MainWindow::set_tree_icons(Fl_Tree *t) {
	static Fl_Pixmap L_close_l(L_close_l_xpm);
	static Fl_Pixmap L_close_r(L_close_r_xpm);
	static Fl_Pixmap L_open(L_open_xpm);
	static Fl_Pixmap L_manual(L_manual_xpm);

	for (Fl_Tree_Item *item = t->first(); item; item=item->next()) {
		if (strcmp(item->label(), label_close_left)==0)
			item->usericon(&L_close_l);
		else if (strcmp(item->label(), label_open)==0)
			item->usericon(&L_open);
		else if (strcmp(item->label(), label_close_right)==0)
			item->usericon(&L_close_r);
		else if (strcmp(item->label(), label_manual)==0)
			item->usericon(&L_manual);

		item->label("");
	}
}

void MainWindow::select_side_type(Fl_Tree *t, void *type) {
	for (Fl_Tree_Item *item = t->first(); item; item=item->next()) {
		if (item->user_data()==type) {
			t->select_only(item);
			return;
		}
	}
}

void MainWindow::create_settings_wnd() {
    settings_wnd = new Fl_Window(320, 480, "Settings");

    settings_wnd->begin();

    len_in = new Fl_Input(70,  10, 90, 25, "L = ");
    n_in   = new Fl_Input(70,  40, 90, 25, "N = ");
    a_in   = new Fl_Input(70,  80, 90, 25, "a = ");
    rho_in = new Fl_Input(70, 110, 90, 25, "rho = ");
    sigma_in = new Fl_Input(70, 140, 90, 25, "sigma = ");

    u_choice = new Fl_Choice(220, 10, 90, 25, "U(x)");
    u_choice->add("- - -");
    u_choice->add("^ - ^");
    u_choice->add("^ - -");
    u_choice->add("- ^ -");
    u_choice->add("- - ^");
    u_choice->add(" V ^ ");
    u_choice->add(" ^ V ");

    p_choice = new Fl_Choice(220, 40, 90, 25, "P(x)");
    p_choice->add("- - -");
    p_choice->add("^ - ^");
    p_choice->add("^ - -");
    p_choice->add("- ^ -");
    p_choice->add("- - ^");

	left_side_choice = new Fl_Tree(90, 200, 52, 116, "Left");
	create_side_selector(left_side_choice);
	left_side_choice->clear();
	left_side_choice->add(label_close_left)->user_data(SIDE_CLOSE);
	left_side_choice->add(label_open)->user_data(SIDE_OPEN);
	left_side_choice->add(label_manual)->user_data(SIDE_MANUAL);
	set_tree_icons(left_side_choice);
	left_side_choice->callback(left_side_cb_st, this);
    bl_in = new Fl_Input(70, 330, 90, 25, "bl");
    cl_in = new Fl_Input(70, 360, 90, 25, "cl");

	right_side_choice = new Fl_Tree(210, 200, 52, 116, "Right");
	create_side_selector(right_side_choice);
	right_side_choice->clear();
	right_side_choice->add(label_close_right)->user_data(SIDE_CLOSE);
	right_side_choice->add(label_open)->user_data(SIDE_OPEN);
	right_side_choice->add(label_manual)->user_data(SIDE_MANUAL);
	set_tree_icons(right_side_choice);
	right_side_choice->callback(right_side_cb_st, this);
    br_in = new Fl_Input(190, 330, 90, 25, "br");
    cr_in = new Fl_Input(190, 360, 90, 25, "cr");

    apply_btn = new Fl_Button(155, 450, 75, 23, "Apply");
    close_btn = new Fl_Button(235, 450, 75, 23, "Close");

    apply_btn->callback(apply_cb_st, this);
    close_btn->callback(close_cb_st, this);

    settings_wnd->end();
    settings_wnd->set_modal();
}

void MainWindow::set_inputs(SettingsHelper helper) {
    static char str[30];

    u_choice->value(helper.un_type);
    p_choice->value(helper.pn_type);
    set_lcond_inputs(helper);
    set_rcond_inputs(helper);

    sprintf(str, "%.4f", helper.L);
    len_in->value(str);

    sprintf(str, "%d", helper.N);
    n_in->value(str);

    sprintf(str, "%.4f", helper.a);
    a_in->value(str);

    sprintf(str, "%.4f", helper.rho);
    rho_in->value(str);

    sprintf(str, "%.4f", helper.sigma);
    sigma_in->value(str);
}

SettingsHelper MainWindow::get_inputs() {
    static char str[30];
    SettingsHelper helper;

    helper.un_type = u_choice->value();
    helper.pn_type = p_choice->value();
    get_lcond_inputs(helper);
    get_rcond_inputs(helper);

    strcpy(str, len_in->value());
    helper.L = atof(str);

    strcpy(str, n_in->value());
    helper.N = atoi(str);

    strcpy(str, a_in->value());
    helper.a = atof(str);

    strcpy(str, rho_in->value());
    helper.rho = atof(str);

    strcpy(str, sigma_in->value());
    helper.sigma = atof(str);

    return helper;
}

void MainWindow::set_lcond_inputs(SettingsHelper helper) {
    static char str[30];
	select_side_type(left_side_choice, helper.left_side_type);

    sprintf(str, "%.1f", helper.bl);
    bl_in->value(str);

    sprintf(str, "%.1f", helper.cl);
    cl_in->value(str);
}

void MainWindow::get_lcond_inputs(SettingsHelper &helper) {
    static char str[30];
	Fl_Tree_Item *item = left_side_choice->first_selected_item();
	helper.left_side_type = item->user_data();

    if (helper.left_side_type==SIDE_MANUAL) {
        strcpy(str, bl_in->value());
        helper.bl = atof(str);

        strcpy(str, cl_in->value());
        helper.cl = atof(str);

	} else if (helper.left_side_type==SIDE_CLOSE) {
		helper.cl = 0.0;
		helper.bl = 1.0;

	} else if (helper.left_side_type==SIDE_OPEN) {
		helper.cl = 0.0;
		helper.bl = 1.0;
	}
}

void MainWindow::set_rcond_inputs(SettingsHelper helper) {
    static char str[30];
	select_side_type(right_side_choice, helper.right_side_type);

    sprintf(str, "%.1f", helper.br);
    br_in->value(str);

    sprintf(str, "%.1f", helper.cr);
    cr_in->value(str);
}

void MainWindow::get_rcond_inputs(SettingsHelper &helper) {
    static char str[30];
	Fl_Tree_Item *item = right_side_choice->first_selected_item();
	helper.right_side_type = item->user_data();

    if (helper.right_side_type==SIDE_MANUAL) {
        strcpy(str, br_in->value());
        helper.br = atof(str);

        strcpy(str, cr_in->value());
        helper.cr = atof(str);

    } else if (helper.right_side_type==SIDE_CLOSE) {
		helper.cr = 0.0;
		helper.br = 1.0;

	} else if (helper.right_side_type==SIDE_OPEN) {
		helper.cr = 0.0;
		helper.br = 1.0;
	}
}

void MainWindow::close_cb_st(Fl_Widget *, void *v) {
    MainWindow *w = (MainWindow *)v;
    w->close_cb();
}

void MainWindow::close_cb() {
    settings_wnd->hide();
}

void MainWindow::apply_cb_st(Fl_Widget *, void *v) {
    MainWindow *w = (MainWindow *)v;
    w->apply_cb();
}

void MainWindow::apply_cb() {
    settings_wnd->hide();
    this->helper(this->get_inputs());
}

void MainWindow::left_side_cb_st(Fl_Widget *, void *v) {
    MainWindow *w = (MainWindow *)v;
    w->left_side_cb();
}

void MainWindow::left_side_cb() {
	Fl_Tree_Item *item = left_side_choice->callback_item();
	Fl_Tree_Reason reason = left_side_choice->callback_reason();
	void *v = item->user_data();

	if (v==SIDE_MANUAL && reason==FL_TREE_REASON_SELECTED) {
		bl_in->show();
		cl_in->show();
	} else {
		bl_in->hide();
		cl_in->hide();
	}
}

void MainWindow::right_side_cb_st(Fl_Widget *, void *v) {
    MainWindow *w = (MainWindow *)v;
    w->right_side_cb();
}

void MainWindow::right_side_cb() {
	Fl_Tree_Item *item = right_side_choice->callback_item();
	Fl_Tree_Reason reason = right_side_choice->callback_reason();
	void *v = item->user_data();

	if (v==SIDE_MANUAL && reason==FL_TREE_REASON_SELECTED) {
		br_in->show();
		cr_in->show();
	} else {
		br_in->hide();
		cr_in->hide();
	}
}

/*
 * Main model options
 */
HazeModel *model;

/*
 * Window controls
 */
MainWindow *window;

/*
 * Main
 */
int main (int argc, char *argv[]) {
	Fl::scheme("gtk+");

    model = new HazeModel(DefL, DefN);
    // model->Reset();

    window = new MainWindow(model);
    window->show(argc, argv);

    int res = Fl::run();
    delete model;
    return res;
}

