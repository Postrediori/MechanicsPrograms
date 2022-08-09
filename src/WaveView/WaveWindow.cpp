#include "pch.h"
#include "SurfaceFunction.h"
#include "WaveModel.h"
#include "WaveWidget.h"
#include "WaveWindow.h"

constexpr double TimerInterval = 0.01;
constexpr size_t BufferLength = 32;

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

    screenshot_btn = new Fl_Button(570, 320, 90, 25, "Screenshot");
    screenshot_btn->callback(screenshot_cb, this);

    auto save_frames_btn = new Fl_Check_Button(570, 350, 90, 25, "Save Frames");
    save_frames_btn->callback(save_frames_cb, this);

    stop_btn->deactivate();
    save_frames_btn->value(saving_frames);

    this->resizable(ww);

    this->end();
}

void WaveWindow::set_inputs() {
    char str[BufferLength] = {0};

    snprintf(str, BufferLength, "%.4f", model.g);
    g_in->value(str);
    snprintf(str, BufferLength, "%.4f", model.h);
    h_in->value(str);
    snprintf(str, BufferLength, "%.4f", model.delta);
    delta_in->value(str);
    snprintf(str, BufferLength, "%.4f", model.epsilon);
    eps_in->value(str);
    snprintf(str, BufferLength, "%.4f", model.dtime);
    dtime_in->value(str);
}

void WaveWindow::get_inputs() {
    char str[BufferLength] = {0};

    strncpy(str, g_in->value(), BufferLength);
    model.g = atof(str);
    strncpy(str, h_in->value(), BufferLength);
    model.h = atof(str);
    strncpy(str, delta_in->value(), BufferLength);
    model.delta = atof(str);
    strncpy(str, eps_in->value(), BufferLength);
    model.epsilon = atof(str);
    strncpy(str, dtime_in->value(), BufferLength);
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

void WaveWindow::screenshot() {
    std::stringstream s;
    s << "frame" << std::setfill('0') << std::setw(4) << frame_counter << ".png";
    ww->screenshot(s.str());

    frame_counter++;
}

void WaveWindow::restart_cb(Fl_Widget* /*w*/, void* v) {
    auto wnd = reinterpret_cast<WaveWindow*>(v);
    wnd->restart();
}

void WaveWindow::restart() {
    get_inputs();
    model.reset();
    ww->redraw();

    animating = false;
    frame_counter = 0;
}

void WaveWindow::step_cb(Fl_Widget* /*w*/, void* v) {
    auto wnd = reinterpret_cast<WaveWindow*>(v);
    wnd->step();
}

void WaveWindow::step() {
    model.step();
    ww->redraw();
    if (saving_frames) {
        screenshot();
    }
}

void WaveWindow::start_cb(Fl_Widget* /*w*/, void* v) {
    auto wnd = reinterpret_cast<WaveWindow*>(v);
    wnd->start();
}

void WaveWindow::start() {
    if (animating) {
        return;
    }
    animating = true;

    restart_btn->deactivate();
    step_btn->deactivate();
    start_btn->deactivate();
    stop_btn->activate();
    screenshot_btn->deactivate();

    Fl::add_timeout(TimerInterval, timer_cb, this);
}

void WaveWindow::stop_cb(Fl_Widget* /*w*/, void* v) {
    auto wnd = reinterpret_cast<WaveWindow*>(v);
    wnd->stop();
}

void WaveWindow::stop() {
    if (!animating) {
        return;
    }
    animating = false;

    restart_btn->activate();
    step_btn->activate();
    start_btn->activate();
    stop_btn->deactivate();
    screenshot_btn->activate();

    Fl::remove_timeout(timer_cb);
}

void WaveWindow::timer_cb(void* v) {
    auto wnd = reinterpret_cast<WaveWindow*>(v);
    wnd->step();
    Fl::repeat_timeout(TimerInterval, timer_cb, v);
}

void WaveWindow::screenshot_cb(Fl_Widget* /*w*/, void* p) {
    auto wnd = reinterpret_cast<WaveWindow*>(p);
    wnd->screenshot();
}

void WaveWindow::save_frames_cb(Fl_Widget* /*w*/, void* p) {
    auto wnd = reinterpret_cast<WaveWindow*>(p);
    wnd->save_frames();
}

void WaveWindow::save_frames() {
    saving_frames = !saving_frames;
}
