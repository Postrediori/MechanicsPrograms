#include "pch.h"
#include "SurfaceFunction.h"
#include "WaveModel.h"
#include "WaveWidget.h"
#include "WaveWindow.h"

/*****************************************************************************
 * Main entry point
 *****************************************************************************/
int main (int argc, char* argv[]) {
#if DRAW_METHOD==DRAW_METHOD_FLTK && !defined(WIN32) && !defined(__APPLE__)
    fl_open_display();
#endif
    auto window = new WaveWindow(700, 500, "Wave Viewer");
    window->show(argc, argv);

    return Fl::run();
}
