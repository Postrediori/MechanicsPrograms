/*
 * FEMSolve
 * v1.2
 */

#include "pch.h"
#include "GraphicsUtils.h"
#include "LinAlgUtils.h"
#include "FEM.h"
#include "ModelWidget.h"
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    auto window = new MainWindow(700, 500, "FEMSolve");
    window->show(argc, argv);
    return Fl::run();
}
