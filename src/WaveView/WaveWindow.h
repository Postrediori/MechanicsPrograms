#pragma once

class WaveWindow : public Fl_Window {
public:
    WaveWindow(int W, int H, const char* l = nullptr);

private:
    void get_inputs();
    void set_inputs();

    void screenshot();

    static void restart_cb(Fl_Widget* w, void* v);
    void restart();

    static void step_cb(Fl_Widget* w, void* v);
    void step();

    static void start_stop_cb(Fl_Widget* w, void* v);
    void start_stop();

    static void timer_cb(void* v);

    static void screenshot_cb(Fl_Widget* w, void* p);

    static void save_frames_cb(Fl_Widget* w, void* p);
    void save_frames();

private:
    WaveModel model;

    WaveWidget* ww{ nullptr };

    Fl_Choice* surface_choice{ nullptr };
    Fl_Input* g_in{ nullptr }, * h_in{ nullptr }, * delta_in{ nullptr },
        * eps_in{ nullptr }, * dtime_in{ nullptr };
    Fl_Button* restart_btn{ nullptr }, * step_btn{ nullptr },
        * start_stop_btn{ nullptr };
    Fl_Button *screenshot_btn{ nullptr };

    bool animating{ false };
    int frame_counter{ 0 };
    bool saving_frames{ false };
};
