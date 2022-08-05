#include "pch.h"
#include "SurfaceFunction.h"
#include "WaveModel.h"
#include "WaveWidget.h"
#include "WaveWindow.h"

constexpr double TimerInterval = 0.01;

WaveWindow::WaveWindow(int W, int H, const char* l)
    : Fl_Window(W, H, l) {
    ww = new WaveWidget(10, 10, 480, 480, &model, "Wave Model");

    surface_choice = new Fl_Choice(570, 10, 90, 25, "Surface :");
    surface_choice->add("Linear");
    surface_choice->add("Sine");
    surface_choice->add("Cosine");
    surface_choice->add("Polygonal");
    surface_choice->value(0);

    g_in = new Fl_Input(570, 40, 90, 25, "g =");
    h_in = new Fl_Input(570, 70, 90, 25, "h =");
    delta_in = new Fl_Input(570, 100, 90, 25, "delta =");
    eps_in = new Fl_Input(570, 130, 90, 25, "eps =");
    dtime_in = new Fl_Input(570, 160, 90, 25, "dtime =");

    set_inputs();

    restart_btn = new Fl_Button(570, 200, 90, 25, "Restart");
    step_btn = new Fl_Button(570, 230, 90, 25, "Step");
    start_btn = new Fl_Button(570, 260, 90, 25, "Start");
    stop_btn = new Fl_Button(570, 290, 90, 25, "Stop");
    restart_btn->callback(restart_cb, this);
    step_btn->callback(step_cb, this);
    start_btn->callback(start_cb, this);
    stop_btn->callback(stop_cb, this);
    stop_btn->deactivate();

    this->resizable(ww);

    this->end();
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

void WaveWindow::restart_cb(Fl_Widget* /*w*/, void* v) {
    WaveWindow* wnd = reinterpret_cast<WaveWindow*>(v);
    wnd->get_inputs();
    wnd->model.reset();
    wnd->ww->redraw();
}

void WaveWindow::step_cb(Fl_Widget* /*w*/, void* v) {
    WaveWindow* wnd = reinterpret_cast<WaveWindow*>(v);
    wnd->model.step();
    wnd->ww->redraw();
}

void WaveWindow::timer_cb(void* v) {
    WaveWindow* wnd = reinterpret_cast<WaveWindow*>(v);
    wnd->model.step();
    wnd->ww->redraw();
    Fl::repeat_timeout(TimerInterval, timer_cb, v);
}

void WaveWindow::start_cb(Fl_Widget* /*w*/, void* v) {
    WaveWindow* wnd = reinterpret_cast<WaveWindow*>(v);
    if (wnd->animating) return;
    wnd->animating = true;

    wnd->restart_btn->deactivate();
    wnd->step_btn->deactivate();
    wnd->start_btn->deactivate();
    wnd->stop_btn->activate();

    Fl::add_timeout(TimerInterval, timer_cb, v);
}

void WaveWindow::stop_cb(Fl_Widget* /*w*/, void* v) {
    WaveWindow* wnd = reinterpret_cast<WaveWindow*>(v);
    if (!wnd->animating) return;
    wnd->animating = false;

    wnd->restart_btn->activate();
    wnd->step_btn->activate();
    wnd->start_btn->activate();
    wnd->stop_btn->deactivate();

    Fl::remove_timeout(timer_cb);
}
