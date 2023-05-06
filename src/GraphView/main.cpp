/*****************************************************************************
 * GraphView
 * Version 6
 * 11.04.2012
 * main.cpp
 ****************************************************************************/

#include "pch.h"
#include "GeometryUtils.h"
#include "OptimizationModel.h"
#include "GraphWidget.h"

/*****************************************************************************
 * Constants
 ****************************************************************************/

const Area FunctionArea = {
    -2.0, -1.0,
    -1.0, 0.0
};

constexpr double Epsilon = 1e-3;


/*****************************************************************************
 * Functions to evaluate
 ****************************************************************************/
const std::vector<std::tuple<Function1D, std::string>> FunctionsData = {
    {[](double x){ return exp(x) + 1 / x; }, "exp(x)+1/x"},
    {[](double x){ return sin(x) + 0.5; }, "sin(x)+0.5"},
    {[](double x){ return sin(x * 10.0) / 5.0 - 2.0 - x; }, "sin(x*10)/5-2-x"}
};


/*****************************************************************************
 * Main window
 ****************************************************************************/
using ModelPtr = std::unique_ptr<Model>;

class MainWindow : public Fl_Double_Window {
public:
    MainWindow(int w, int h, const char* lbl = nullptr);

private:
    static void SearchCb(Fl_Widget *w, void *p);
    void SearchFunc();

    static void TimerCb(void *p);
    void TimerFunc();

    static void SetModelCb(Fl_Widget *w, void *p);
    void SetModelFunc();

    static void SetFuncCb(Fl_Widget *w, void *p);
    void SetFunc(int n);

    void StopSearch();
    void Refresh();

    int GetSelectedModelId() const;

    const ModelPtr& GetCurrentModel() const { return models[currentModel]; }

private:
    double timerInterval = 0.0025;

    int currentFunction = 0;

    int currentModel = 0;
    std::vector<ModelPtr> models;

    GraphWidget *graph = nullptr;
    Fl_Text_Buffer *buffer = nullptr;
    std::vector<Fl_Button*> modelButtons;
};

MainWindow::MainWindow(int w, int h, const char* lbl)
    : Fl_Double_Window(w, h, lbl) {

    models.push_back(std::make_unique<ScanModel>());
    models.push_back(std::make_unique<FibModel>());
    models.push_back(std::make_unique<GradModel>());
    models.push_back(std::make_unique<BounceModel>());
    models.push_back(std::make_unique<BolzanoModel>());
    currentModel = 0;

    resizable(this);

    // Controls setup begin
    begin();

    graph = new GraphWidget(10, 10, 480, 480, "Graph Widget");
    graph->set_model(models[currentModel]);

    auto functionChoice = new Fl_Choice(560, 10, 120, 25, "Function");
    int k = 0;
    for (const auto& funcData : FunctionsData) {
        auto s = std::to_string(k);
        functionChoice->add(s.c_str()); // Add and replace label because Fl_Choice uses division sign as special character for submenu
        functionChoice->replace(k, std::get<1>(funcData).c_str());
        k++;
    }
    functionChoice->value(0);
    functionChoice->callback(MainWindow::SetFuncCb, static_cast<void*>(this));

    // Model chooser
    auto modelChooser = new Fl_Group(500, 65, 190, 200, "Select model");
    modelChooser->box(FL_SHADOW_BOX);
    modelChooser->begin();

    int y = modelChooser->y() + 10;
    for (const auto& model : models) {
        auto btn = new Fl_Round_Button(510, y, 170, 25, model->name());
        btn->type(FL_RADIO_BUTTON);
        btn->callback(MainWindow::SetModelCb, static_cast<void *>(this));
        modelButtons.push_back(static_cast<Fl_Button*>(btn));
        y += 35;
    }

    modelButtons[0]->set();

    modelChooser->end();

    auto btn = new Fl_Button(550, 275, 90, 25, "Search");
    btn->callback(MainWindow::SearchCb, static_cast<void *>(this));

    // Output
    buffer = new Fl_Text_Buffer(1024);

    auto out = new Fl_Text_Display(500, 310, 190, 180);
    out->buffer(buffer);

    // Controls setup end
    end();

    SetFunc(0);
}

void MainWindow::SearchCb(Fl_Widget * /*w*/, void *p) {
    auto window = reinterpret_cast<MainWindow *>(p);
    window->SearchFunc();
}

void MainWindow::SearchFunc() {
    if (GetCurrentModel()->search()) {
        StopSearch();
    }

    GetCurrentModel()->start();
    Fl::add_timeout(timerInterval, TimerCb, static_cast<void *>(this));
}

void MainWindow::TimerCb(void *p) {
    auto window = reinterpret_cast<MainWindow *>(p);
    window->TimerFunc();
}

void MainWindow::TimerFunc() {
    GetCurrentModel()->step();
    Refresh();
    if (GetCurrentModel()->search()) {
        Fl::repeat_timeout(timerInterval, MainWindow::TimerCb, static_cast<void*>(this));
    }
}

void MainWindow::SetFuncCb(Fl_Widget *w, void *p) {
    auto window = reinterpret_cast<MainWindow *>(p);
    auto c = reinterpret_cast<Fl_Choice*>(w);

    auto val = c->value();
    window->SetFunc(val);
}

void MainWindow::SetFunc(int n) {
    if (GetCurrentModel()->search()) {
        StopSearch();
    }

    currentFunction = n;

    auto funcData = FunctionsData[currentFunction];
    auto func = std::get<0>(funcData);

    for (auto& m : models) {
        m->init_search(FunctionArea, Epsilon, func);
        m->reset();
    }

    graph->set_plot(func, FunctionArea);
    graph->redraw();
    Refresh();
}

void MainWindow::SetModelCb(Fl_Widget * /*w*/, void *p) {
    auto window = reinterpret_cast<MainWindow *>(p);
    window->SetModelFunc();
}

void MainWindow::SetModelFunc() {
    if (GetCurrentModel()->search()) {
        StopSearch();
    }

    currentModel = GetSelectedModelId();

    timerInterval = GetCurrentModel()->timer_interval();
    GetCurrentModel()->reset();
    graph->set_model(GetCurrentModel());
    Refresh();
}

void MainWindow::StopSearch() {
    Fl::remove_timeout(MainWindow::TimerCb);
    GetCurrentModel()->stop();
}

void MainWindow::Refresh() {
    graph->redraw();

    buffer->select(0, buffer->length());
    buffer->remove_selection();

    auto funcData = FunctionsData[currentFunction];
    auto funcName = std::get<1>(funcData);

    const auto& model = GetCurrentModel();

    std::stringstream s;
    s << "Funcion: " << funcName << std::endl <<
        "Epsilon: " << std::fixed << std::setprecision(6) << Epsilon << std::endl <<
        "Min X: " << std::fixed << std::setprecision(6) << model->x1() << std::endl <<
        "Min Y: " << std::fixed << std::setprecision(6) << model->y1() << std::endl <<
        "FuncCalcCount: " << model->counter() << std::endl <<
        "Status: " << (model->search() ? "Searching" : "Complete") << std::endl;

    buffer->insert(0, s.str().c_str());
}

int MainWindow::GetSelectedModelId() const {
    int k = 0;
    for (const auto& btn : modelButtons) {
        if (btn->value() == 1) {
            return k;
        }
        k += 1;
    }
    return -1; // Wrong selection
}


/*****************************************************************************
 * Main program
 ****************************************************************************/

int main(int /*argc*/, const char* /*argv*/[]) {
#if !defined(WIN32) && !defined(__APPLE__)
    fl_open_display();
#endif

    auto window = new MainWindow(700, 500, "One-dimensional Optimization Methods");
    window->show();

    int res = Fl::run();
    return res;
}
