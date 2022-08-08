#include "pch.h"
#include "MathUtils.h"
#include "FuncUtils.h"
#include "GraphUtils.h"
#include "Simplex.h"
#include "SearchEngine.h"
#include "SimplexEngine.h"
#include "GaussSEngine.h"
#include "DescentEngine.h"
#include "RelaxationEngine.h"
#include "ScanEngine.h"
#include "ContourPlot.h"
#include "GraphWidget.h"
#include "MainWindow.h"

constexpr double TimerInterval = 1.0;

MainWindow::MainWindow(int w, int h, const char* title)
        : Fl_Window(w, h, title) {
    begin();

    engines_.push_back({"Nelder-Mead method", std::make_unique<SimplexEngine>()});
    engines_.push_back({"Gauss-Seidel method", std::make_unique<GaussSEngine>()});
    engines_.push_back({"Gradient descent", std::make_unique<DescentEngine>()});
    engines_.push_back({"Coordinate descent", std::make_unique<RelaxationEngine>()});
    engines_.push_back({"Scaning", std::make_unique<ScanEngine>()});
    current_engine_ = 0;

    graph_ = new GraphWidget(5, 5, 490, 490, std::get<1>(engines_.at(current_engine_)).get(), "Plot of F(x,y)");

    buffer_ = new Fl_Text_Buffer(1024);

    auto search_step_btn = new Fl_Button(500, 5, 75, 23, "Search");
    search_step_btn->callback(MainWindow::search_step_cb, static_cast<void*>(this));

    auto search_stop_btn = new Fl_Button(580, 5, 75, 23, "Stop");
    search_stop_btn->callback(MainWindow::search_stop_cb, static_cast<void*>(this));

    auto output = new Fl_Text_Display(500, 35, 195, 200);
    output->buffer(buffer_);

    auto search_engines = new Fl_Group(500, 250, 195, 200, "Search Engines");
    search_engines->box(FL_SHADOW_BOX);
    search_engines->begin();

    for (int i = 0; i < engines_.size(); i++) {
        auto engine_btn = new Fl_Round_Button(
            510, 260 + 30 * i,
            175, 23,
            std::get<0>(engines_.at(i)).c_str());
        engine_btn->type(FL_RADIO_BUTTON);
        engine_btn->callback(set_engine_cb, reinterpret_cast<void*>(i));

        if (i == 0) {
            engine_btn->set();
        }
    }

    search_engines->end();

    resizable(graph_);

    end();
}

void MainWindow::refresh() {
    constexpr size_t BufferLength = 256;
    char string[BufferLength] = { 0 };

    const auto& engine = std::get<1>(engines_.at(current_engine_));

    buffer_->select(0, buffer_->length());
    buffer_->remove_selection();
    snprintf(string, BufferLength - 1,
        "Epsilon: %f\n"
        "XMIN: %f\n"
        "YMIN: %f\n"
        "ZMIN: %f\n"
        "COUNT: %d\n"
        "STATUS: %s\n",
        Epsilon,
        engine->xmin(),
        engine->ymin(),
        engine->zmin(),
        engine->count(),
        (engine->search_over()) ? "Complete" : "Searching"
    );
    buffer_->append(string);

    graph_->redraw();
}

void MainWindow::search_start() {
    std::get<1>(engines_.at(current_engine_))->search_start();
}

void MainWindow::search_step() {
    std::get<1>(engines_.at(current_engine_))->search_step();
    refresh();
}

bool MainWindow::search_over() {
    return std::get<1>(engines_.at(current_engine_))->search_over();
}

void MainWindow::engine(int idx) {
    if (idx < 0 || idx >= engines_.size()) {
        std::cerr << "Error: Incorrect engine ID " << idx << std::endl;
        return;
    }
    current_engine_ = idx;
    auto& e = std::get<1>(engines_.at(current_engine_));
    e->search_start();
    graph_->engine(e.get());
    refresh();
}

void MainWindow::search_step_cb(Fl_Widget*, void* p) {
    Fl::remove_timeout(MainWindow::timer_cb);

    auto window = static_cast<MainWindow*>(p);
    window->search_step();

    Fl::add_timeout(TimerInterval, MainWindow::timer_cb, p);
}

void MainWindow::search_stop_cb(Fl_Widget*, void*) {
    Fl::remove_timeout(timer_cb);
}

void MainWindow::timer_cb(void* p) {
    auto window = static_cast<MainWindow*>(p);
    window->search_step();

    if (!window->search_over()) {
        Fl::repeat_timeout(TimerInterval, timer_cb, p);
    }
}

void MainWindow::set_engine_cb(Fl_Widget* w, void* p) {
    Fl::remove_timeout(timer_cb);

    auto window = static_cast<MainWindow*>(w->parent()->parent());
    auto engine = static_cast<int>(reinterpret_cast<uintptr_t>(p));

    window->engine(engine);
}
