#pragma once

class WaveWindow : public Fl_Window {
public:
    WaveWindow(int W, int H, const char* l = nullptr);

private:
    void get_inputs();
    void set_inputs();

    static void restart_cb(Fl_Widget* w, void* v);
    static void step_cb(Fl_Widget* w, void* v);
    static void timer_cb(void* v);
    static void start_cb(Fl_Widget* w, void* v);
    static void stop_cb(Fl_Widget* w, void* v);

private:
    WaveModel model;

    WaveWidget* ww{ nullptr };

    Fl_Choice* surface_choice{ nullptr };
    Fl_Input* g_in{ nullptr }, * h_in{ nullptr }, * delta_in{ nullptr },
        * eps_in{ nullptr }, * dtime_in{ nullptr };
    Fl_Button* restart_btn{ nullptr }, * step_btn{ nullptr },
        * start_btn{ nullptr }, * stop_btn{ nullptr };

    bool animating{ false };
};
