/*
 * SimplexView
 * v1.6
 */

#include "pch.h"
#include "MathUtils.h"
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

int main(int argc, char* argv[]) {
#if !defined(DRAW_OPENGL) && !defined(WIN32) && !defined(__APPLE__)
    fl_open_display();
#endif

    auto window = new MainWindow(700, 500, "Optimization Methods");
    window->show(argc, argv);
    return Fl::run();
}
