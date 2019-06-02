/*
 * FEMSolve
 * v1.2
 */

#include <stdlib.h>
#include <stdio.h>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

#include "main.h"
#include "matrix.h"
#include "model.h"
#include "modelwidget.h"

FinitModel model;

ModelWidget *model_widget;

Fl_Window *report_dlg;
Fl_Text_Buffer *buffer;

void load_model(const char *filename);
void solve_model();

void open_cb(Fl_Widget *, void *);
void exit_cb(Fl_Widget *, void *);
void solve_cb(Fl_Widget *, void *);
void report_cb(Fl_Widget *, void *);
void report_close_cb(Fl_Widget *, void *);
void about_cb(Fl_Widget *, void *);

int main(int argc, char **argv) {
    buffer = new Fl_Text_Buffer(1024);

    // Report window
    report_dlg = new Fl_Window(480, 320, "Report");
    report_dlg->resizable(report_dlg);
    report_dlg->begin();

    Fl_Text_Display output(2, 2, 480-4, 295-4);
    output.buffer(buffer);

    Fl_Button report_close_btn(400, 295, 75, 23, "Close");
    report_close_btn.callback(report_close_cb);

    report_dlg->end();
    report_dlg->set_non_modal();

    // Main window
    Fl_Window window(700, 500, "FEMSolve");
    window.resizable(&window);
    window.begin();

    // Create main menu
    Fl_Menu_Item menu_items[] = {
        {"&File", 0, 0, 0, FL_SUBMENU},
        {"&Open file...", FL_COMMAND+'o', (Fl_Callback *)open_cb, 0, FL_MENU_DIVIDER},
        {"E&xit", FL_COMMAND+'x', (Fl_Callback *)exit_cb, 0},
        {0},

        {"&Solution", 0, 0, 0, FL_SUBMENU},
        {"Sol&ve", FL_COMMAND+'s', (Fl_Callback *)solve_cb, 0, FL_MENU_DIVIDER},
        {"Solution &report", FL_COMMAND+'r', (Fl_Callback *)report_cb, 0},
        {0},

        {"&Help", 0, 0, 0, FL_SUBMENU},
        {"A&bout...", 0, (Fl_Callback *)about_cb, 0},
        {0},

        {0}
    };
    Fl_Menu_Bar menu_bar(0, 0, 700, 30);
    menu_bar.copy(menu_items);

    model_widget = new ModelWidget(2, 32, 700-4, 500-34, &model);

    window.end();
    window.show(argc, argv);
    int res = Fl::run();

    // delete report_dlg;

    return res;
}

void open_cb(Fl_Widget *, void *) {
    Fl_Native_File_Chooser fnfc;
    fnfc.title("Pick a file");
    fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
    fnfc.filter("Text\t*.txt");
    switch (fnfc.show()) {
    case -1:
        fl_message("Error opening a file\n%s", fnfc.errmsg());
        break;

    case 1:
        return;

    default:
        fl_message("Picked file:\n%s", fnfc.filename());
        load_model(fnfc.filename());
        break;
    }
}

void exit_cb(Fl_Widget *, void *) {
    exit(0);
}

void solve_cb(Fl_Widget *, void *) {
    solve_model();
}

void report_cb(Fl_Widget *, void *) {
    report_dlg->show();
}

void report_close_cb(Fl_Widget *, void *) {
    report_dlg->hide();
}

void load_model(const char *filename) {
    model.load_from_file(filename);
    model_widget->reload_model();
    model_widget->redraw();
}

void solve_model() {
    model.prepare_solve();
    /*printf("A:\n");
    print_matrix(model.a, model.component_count);
    printf("B:\n");
    print_vector(model.b, model.component_count);*/

    model.solve();
    /*printf("U:\n");
    print_vector(model.u, model.component_count);
    printf("N:\n");
    print_vector(model.n, model.elem_count);*/

    fl_message("Solution is done!");

    buffer->select(0, buffer->length());
    buffer->remove_selection();

    char str[256];

    buffer->append("Node displacements:\n");
    for (int i=0; i<model.node_count; i++) {
        int node = model.nodes[i].node;
        sprintf(str, "%d\t%12.4e\t%12.4e\n", node,
                model.u[(node-1)*2],
                model.u[(node-1)*2 + 1]
               );
        buffer->append(str);
    }
    buffer->append("\n");

    buffer->append("Element forces:\n");
    for (int i=0; i<model.elem_count; i++) {
        sprintf(str, "%d\t%12.4e\n", model.elems[i].elem, model.n[i]);
        buffer->append(str);
    }
    buffer->append("\n");
}

void about_cb(Fl_Widget *, void *) {
    fl_message("FEMSolve v%s\n"
               "Finite Element Model Solver\n"
               "by Rasim Labibov, 2012\n"
               "Build on %s, %s\n"
               "FLTK v%d.%d.%d",
               FEMSOLVE_VERSION,
               __DATE__, __TIME__,
               FL_MAJOR_VERSION, FL_MINOR_VERSION, FL_PATCH_VERSION);
}
