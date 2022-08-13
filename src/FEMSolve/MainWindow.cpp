#include "pch.h"
#include "GraphicsUtils.h"
#include "LinAlgUtils.h"
#include "FEM.h"
#include "ModelWidget.h"
#include "MainWindow.h"

#include "icon32_xpm.h"

constexpr auto FEMSOLVE_VERSION = "1.2";

MainWindow::MainWindow(int w, int h, const char* l)
    : Fl_Window(w, h, l) {

    buffer = new Fl_Text_Buffer(1024);

    // Report window
    report_dlg = new Fl_Window(480, 320, "Report");
    report_dlg->resizable(report_dlg);
    report_dlg->begin();

    auto output = new Fl_Text_Display(2, 2, 480 - 4, 295 - 4);
    output->buffer(buffer);

    auto report_close_btn = new Fl_Button(400, 295, 75, 23, "Close");
    report_close_btn->callback(report_close_cb);

    report_dlg->end();
    report_dlg->set_non_modal();

    // Main window
    begin();

    // Create main menu
    static const std::vector<Fl_Menu_Item> menu_items = {
        {"&File", 0, 0, 0, FL_SUBMENU},
        {"&Open file...", FL_COMMAND + 'o', open_cb, static_cast<void*>(this), FL_MENU_DIVIDER},
        {"E&xit", FL_COMMAND + 'x', exit_cb, 0},
        {0},

        {"&Solution", 0, 0, 0, FL_SUBMENU},
        {"Sol&ve", FL_COMMAND + 's', solve_cb, static_cast<void*>(this), FL_MENU_DIVIDER},
        {"Solution &report", FL_COMMAND + 'r', report_cb, static_cast<void*>(this)},
        {0},

        {"&Help", 0, 0, 0, FL_SUBMENU},
        {"A&bout...", 0, about_cb, 0},
        {0},

        {0}
    };
    auto menu_bar = new Fl_Menu_Bar(0, 0, 700, 30);
    menu_bar->copy(menu_items.data());

    model_widget = new ModelWidget(2, 32, 700 - 4, 500 - 34, model);

    end();

    // Set window icon
    Fl_Pixmap icon32(icon32_xpm);
    Fl_RGB_Image imageIcon(&icon32, FL_WHITE);
    icon(&imageIcon);

    resizable(model_widget);
}

void MainWindow::open_cb(Fl_Widget*, void* p) {
    auto w = static_cast<MainWindow*>(p);
    w->open();
}

void MainWindow::open() {
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

void MainWindow::exit_cb(Fl_Widget*, void*) {
    exit(0);
}

void MainWindow::solve_cb(Fl_Widget*, void* p) {
    auto w = static_cast<MainWindow*>(p);
    w->solve();
}

void MainWindow::solve() {
    solve_model();
}

void MainWindow::report_cb(Fl_Widget*, void* p) {
    auto w = static_cast<MainWindow*>(p);
    w->report();
}

void MainWindow::report() {
    report_dlg->show();
}

void MainWindow::report_close_cb(Fl_Widget*, void* p) {
    auto w = static_cast<MainWindow*>(p);
    w->report_close();
}

void MainWindow::report_close() {
    report_dlg->hide();
}

void MainWindow::load_model(const char* filename) {
    model.load_from_file(filename);
    model_widget->reload_model();
    model_widget->redraw();
}

void MainWindow::solve_model() {
    model.prepare_solve();

    std::cout << "A : " << std::endl;
    std::cout << model.a << std::endl << std::endl;

    std::cout << "B : " << std::endl;
    std::cout << model.b << std::endl << std::endl;

    model.solve();

    std::cout << "U : " << std::endl;
    std::cout << model.u << std::endl << std::endl;

    std::cout << "N : " << std::endl;
    std::cout << model.n << std::endl << std::endl;

    fl_message("Solution is done!");

    buffer->select(0, buffer->length());
    buffer->remove_selection();

    constexpr size_t BufferLen = 256;
    char str[BufferLen] = { 0 };

    buffer->append("Node displacements:\n");
    for (int i = 0; i < model.nodes.size(); i++) {
        int node = model.nodes[i].node;
        snprintf(str, BufferLen-1, "%d\t%12.4e\t%12.4e\n", node,
            model.u[(node - 1) * 2],
            model.u[(node - 1) * 2 + 1]
        );
        buffer->append(str);
    }
    buffer->append("\n");

    buffer->append("Element forces:\n");
    for (int i = 0; i < model.elems.size(); i++) {
        snprintf(str, BufferLen-1, "%d\t%12.4e\n", model.elems[i].elem, model.n[i]);
        buffer->append(str);
    }
    buffer->append("\n");
}

void MainWindow::about_cb(Fl_Widget*, void*) {
    fl_message("FEMSolve v%s\n"
        "Finite Element Model Solver\n"
        "by Rasim Labibov, 2012\n"
        "Build on %s, %s\n"
        "FLTK v%d.%d.%d",
        FEMSOLVE_VERSION,
        __DATE__, __TIME__,
        FL_MAJOR_VERSION, FL_MINOR_VERSION, FL_PATCH_VERSION);
}
