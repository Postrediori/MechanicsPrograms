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

    auto window = new MainWindow(&model);
    window->show(argc, argv);

    return Fl::run();
}
