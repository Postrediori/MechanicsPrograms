#pragma once

class MainWindow : public Fl_Window {
public:
    MainWindow(int w, int h, const char* l = nullptr);

private:
    void load_model(const char* filename);
    void solve_model();

    static void open_cb(Fl_Widget*, void*);
    void open();

    static void exit_cb(Fl_Widget*, void*);

    static void solve_cb(Fl_Widget*, void*);
    void solve();

    static void report_cb(Fl_Widget*, void*);
    void report();

    static void report_close_cb(Fl_Widget*, void*);
    void report_close();

    static void about_cb(Fl_Widget*, void*);

private:
    FinitModel model;

    ModelWidget* model_widget{ nullptr };

    Fl_Window* report_dlg{ nullptr };
    Fl_Text_Buffer* buffer{ nullptr };
};
