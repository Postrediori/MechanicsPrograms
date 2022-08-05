/*
 * MediaWave
 */

#include "pch.h"
#include "MediumModel.h"
#include "PlotWidget.h"
#include "MainWindow.h"


/*
 * Main
 */
int main (int argc, char *argv[]) {
    Fl::scheme("gtk+");

    MediumModel model(DefL, DefN);
    // model.Reset();

#if !defined(DRAW_OPENGL) && !defined(WIN32) && !defined(__APPLE__)
    fl_open_display();
#endif

    auto window = new MainWindow(&model);
    window->show(argc, argv);

    return Fl::run();
}
