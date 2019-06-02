/*
 * SimplexView
 * v1.6
 */

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Round_Button.H>

#include "Simplex.h"
#include "SimplexEngine.h"
#include "GaussSEngine.h"
#include "DescentEngine.h"
#include "RelaxationEngine.h"
#include "ScanEngine.h"
#include "GraphWidget.h"

static const double TimerInterval = 1.0;

SimplexEngine simplex_engine;
GaussSEngine gausss_engine;
DescentEngine descent_engine;
RelaxationEngine relax_engine;
ScanEngine scan_engine;
SearchEngine *engine = &simplex_engine;

Fl_Text_Buffer *buffer;
GraphWidget *graph;

void refresh();

void search_step_cb(Fl_Widget *, void *);
void search_stop_cb(Fl_Widget *, void *);
void timer_cb(void *);
void set_engine_cb(Fl_Widget *, void *);

int main(int argc, char **argv) {
    Fl_Window window(700, 500, "SimplexView");
    window.resizable(&window);
    window.begin();

    graph = new GraphWidget(5, 5, 490, 490, engine);

    Fl_Button search_step_btn(500, 5, 75, 23, "Search");
    search_step_btn.callback(search_step_cb);

    Fl_Button search_stop_btn(580, 5, 75, 23, "Stop");
    search_stop_btn.callback(search_stop_cb);

    Fl_Text_Display output(500, 35, 195, 200);
    buffer = new Fl_Text_Buffer(1024);
    output.buffer(buffer);

    Fl_Group search_engines(500, 250, 195, 200, "Search Engines");
    search_engines.box(FL_SHADOW_BOX);
    search_engines.begin();

    Fl_Round_Button simplex_engine_btn(510, 260, 175, 23, "Nelder-Mead method");
    simplex_engine_btn.type(FL_RADIO_BUTTON);
    simplex_engine_btn.callback(set_engine_cb, &simplex_engine);
    simplex_engine_btn.set();

    Fl_Round_Button gausss_engine_btn(510, 290, 175, 23, "Gauss-Seidel method");
    gausss_engine_btn.type(FL_RADIO_BUTTON);
    gausss_engine_btn.callback(set_engine_cb, &gausss_engine);

    Fl_Round_Button descent_engine_btn(510, 320, 175, 23, "Gradient descent");
    descent_engine_btn.type(FL_RADIO_BUTTON);
    descent_engine_btn.callback(set_engine_cb, &descent_engine);

    Fl_Round_Button relax_engine_btn(510, 350, 175, 23, "Coordinate descent");
    relax_engine_btn.type(FL_RADIO_BUTTON);
    relax_engine_btn.callback(set_engine_cb, &relax_engine);

    Fl_Round_Button scan_engine_btn(510, 380, 175, 23, "Scaning");
    scan_engine_btn.type(FL_RADIO_BUTTON);
    scan_engine_btn.callback(set_engine_cb, &scan_engine);

    search_engines.end();

    window.end();
    window.show(argc, argv);
    return Fl::run();
}

void search_step_cb(Fl_Widget *, void *) {
    Fl::remove_timeout(timer_cb);

    engine->search_start();

    Fl::add_timeout(TimerInterval, timer_cb);
}

void search_stop_cb(Fl_Widget *, void *) {
    Fl::remove_timeout(timer_cb);
}

void timer_cb(void *) {
    engine->search_step();

    refresh();
    if (!engine->search_over()) Fl::repeat_timeout(TimerInterval, timer_cb);
}

void set_engine_cb(Fl_Widget *, void *data) {
    SearchEngine *e = (SearchEngine *) data;
    Fl::remove_timeout(timer_cb);
    engine = e;
    graph->engine(e);
    refresh();
}

void refresh() {
    buffer->select(0, buffer->length());
    buffer->remove_selection();
    char string[256];
    sprintf(string,
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
    buffer->append(string);

    graph->redraw();
    graph->invalidate();
}
