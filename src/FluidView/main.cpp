#include "pch.h"
#include "FluidModel.h"
#include "GraphWidget.h"

// Calculate function callback
struct CalcFuncCbData {
    GraphWidget* graph;
    Fl_Text_Buffer* buffer;
    Fl_Input* inpq;
};

void calcFunc(Fl_Widget* /*w*/, void* p) {
    assert(p);

    auto data = static_cast<CalcFuncCbData*>(p);
    auto inpq = data->inpq;
    auto buffer = data->buffer;
    auto graph = data->graph;

    char text[32];
    strncpy(text, inpq->value(), sizeof(text));
    double q = atof(text);

    double l1, l2;
    l1 = lambdafunc(q, 1);
    l2 = lambdafunc(q, 2);

    buffer->select(0, buffer->length());
    buffer->remove_selection();

    char str[256];
    sprintf(str, "Lambda1: %12.4f\n"
        "Lambda2: %12.4f\n"
        "Epsilon: %f",
        l1, l2, Epsilon);

    buffer->insert(0, str);

    graph->set_lines(q, l1, l2);
    graph->redraw();
}

// Main entry point
int main(int argc, char **argv) {
    auto window = new Fl_Window(700, 500, "Fluid View");
    window->begin();

    auto graph = new GraphWidget(10, 10, 480, 480);

    auto inpq = new Fl_Input(575, 10, 90, 25, "q = ");
    auto btnCalc = new Fl_Button(500, 40, 90, 25, "Calculate");
    auto disp = new Fl_Text_Display(500, 70, 190, 200);
    auto buffer = new Fl_Text_Buffer(1024);
    disp->buffer(buffer);

    CalcFuncCbData cbData = { graph , buffer , inpq };
    btnCalc->callback(calcFunc, &cbData);

    window->end();
    window->show(argc, argv);

    // Initial setup
    inpq->value("0.5");
    calcFunc(nullptr, &cbData);

    return Fl::run();
}
