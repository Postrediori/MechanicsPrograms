#pragma once

/*
 * Settings
 */
enum class SideType : int {
    Closed = 1,
    Open = 2,
    Manual = 3
};

struct SettingsHelper {
    float L{ 0.0f };
    int N{ 0 };

    float a{ 0.0f };
    float rho{ 0.0f };
    float sigma{ 0.0f };

    int un_type{ 0 };
    int pn_type{ 0 };

    SideType left_side_type{ SideType::Closed };
    SideType right_side_type{ SideType::Closed };

    float bl{ 0.0f }, cl{ 0.0f };
    float br{ 0.0f }, cr{ 0.0f };
};

/*
 * Main Window
 */
const double TimerInterval = 0.01;

class MainWindow : public Fl_Window {
public:
    MainWindow(MediumModel*);

    void helper(SettingsHelper);
    SettingsHelper helper();

private:
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

    static void create_side_selector(Fl_Tree*);
    static void set_tree_icons(Fl_Tree*);
    static void select_side_type(Fl_Tree*, SideType);
    void create_settings_wnd();

    static void close_cb_st(Fl_Widget*, void*);
    void close_cb();

    static void apply_cb_st(Fl_Widget*, void*);
    void apply_cb();

    static void left_side_cb_st(Fl_Widget*, void*);
    void left_side_cb();

    static void right_side_cb_st(Fl_Widget*, void*);
    void right_side_cb();

    static void switch_bar_plot_cb(Fl_Widget* w, void* p);
    void switch_bar_plot();

    void set_inputs(SettingsHelper);
    SettingsHelper get_inputs();

    void set_lcond_inputs(SettingsHelper helper);
    void get_lcond_inputs(SettingsHelper& helper);

    void set_rcond_inputs(SettingsHelper helper);
    void get_rcond_inputs(SettingsHelper& helper);

private:
    bool bar_plot_{ false };

    MediumModel *model_{ nullptr };

    PlotWidget* uw{ nullptr };
    PlotWidget* pw{ nullptr };
    Fl_Button* step_btn{ nullptr };
    Fl_Button* start_btn{ nullptr };
    Fl_Button* stop_btn{ nullptr };
    Fl_Button* settings_btn{ nullptr };

    // Settings window properties
    SettingsHelper helper_;
    Fl_Window* settings_wnd{ nullptr };

    Fl_Button* close_btn{ nullptr }, * apply_btn{ nullptr };

    Fl_Input* len_in{ nullptr }, * n_in{ nullptr };
    Fl_Input* a_in{ nullptr }, * rho_in{ nullptr }, * sigma_in{ nullptr };
    Fl_Choice* u_choice{ nullptr }, * p_choice{ nullptr };
    Fl_Tree* left_side_choice{ nullptr }, * right_side_choice{ nullptr };
    Fl_Input* bl_in{ nullptr }, * cl_in{ nullptr }, * br_in{ nullptr }, * cr_in{ nullptr };
    Fl_Check_Button* bar_plot_check{ nullptr };
};
