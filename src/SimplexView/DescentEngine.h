// DescentEngine.h
#ifndef DESCENT_ENGINE_H
#define DESCENT_ENGINE_H

#include "SearchEngine.h"

class DescentEngine: public SearchEngine {
    double x, y;
    double ddx, ddy;
    double xold, yold;

public:
    DescentEngine();

    void draw();

    void search_start();
    void search_step();
};

#endif
// DESCENT_ENGINE_H
