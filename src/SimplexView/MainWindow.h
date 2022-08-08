#pragma once

class MainWindow : public Fl_Window {
public:
    MainWindow(int w, int h, const char* title);

    void refresh();

    void engine(int idx);
    void search_start();
    void search_step();
    bool search_over();

    static void search_step_cb(Fl_Widget*, void*);
    static void search_stop_cb(Fl_Widget*, void*);
    static void timer_cb(void*);
    static void set_engine_cb(Fl_Widget*, void*);

private:
    std::vector<std::tuple<std::string, std::unique_ptr<SearchEngine>>> engines_;
    int current_engine_{ 0 };

    GraphWidget* graph_{ nullptr };
    Fl_Text_Buffer* buffer_{ nullptr };
};
