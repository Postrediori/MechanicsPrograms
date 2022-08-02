#pragma once

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

    void* left_side_type;
    void* right_side_type;

    float bl, cl;
    float br, cr;
};

/*
 * Main Window
 */
static const double TimerInterval = 0.01;

class MainWindow : public Fl_Window {
private:
    MediumModel* model_;

    PlotWidget* uw, * pw;
    Fl_Button* step_btn, * start_btn, * stop_btn, * settings_btn;

    void redraw();

    static void step_cb_st(Fl_Widget*, void*);
    void step_cb();

    static void start_cb_st(Fl_Widget*, void*);
    void start_cb();

    static void stop_cb_st(Fl_Widget*, void*);
    void stop_cb();

    static void settings_cb_st(Fl_Widget*, void*);
    void settings_cb();

    static void timer_cb_st(void*);
    void timer_cb();

    // Settings window properties
    SettingsHelper helper_;
    Fl_Window* settings_wnd;

    Fl_Button* close_btn, * apply_btn;

    Fl_Input* len_in, * n_in;
    Fl_Input* a_in, * rho_in, * sigma_in;
    Fl_Choice* u_choice, * p_choice;
    Fl_Tree* left_side_choice, * right_side_choice;
    Fl_Input* bl_in, * cl_in, * br_in, * cr_in;

    static void create_side_selector(Fl_Tree*);
    static void set_tree_icons(Fl_Tree*);
    static void select_side_type(Fl_Tree*, void*);
    void create_settings_wnd();

    static void close_cb_st(Fl_Widget*, void*);
    void close_cb();

    static void apply_cb_st(Fl_Widget*, void*);
    void apply_cb();

    static void left_side_cb_st(Fl_Widget*, void*);
    void left_side_cb();

    static void right_side_cb_st(Fl_Widget*, void*);
    void right_side_cb();

    void set_inputs(SettingsHelper);
    SettingsHelper get_inputs();

    void set_lcond_inputs(SettingsHelper helper);
    void get_lcond_inputs(SettingsHelper& helper);

    void set_rcond_inputs(SettingsHelper helper);
    void get_rcond_inputs(SettingsHelper& helper);

public:
    MainWindow(MediumModel*);

    void helper(SettingsHelper);
    SettingsHelper helper();
};
