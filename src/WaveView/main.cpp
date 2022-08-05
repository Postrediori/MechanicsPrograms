#include "pch.h"
#include "SurfaceFunction.h"
#include "WaveModel.h"
#include "WaveWidget.h"
#include "WaveWindow.h"

/*****************************************************************************
 * Main entry point
 *****************************************************************************/
int main (int argc, char* argv[]) {
    auto window = new WaveWindow(700, 500, "Wave Viewer");
    window->show(argc, argv);

    return Fl::run();
}
