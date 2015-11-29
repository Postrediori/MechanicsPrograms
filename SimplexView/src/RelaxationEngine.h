// RelaxationEngine.h
#ifndef RELAXATION_ENGINE_H
#define RELAXATION_ENGINE_H

#include "SearchEngine.h"

class RelaxationEngine: public SearchEngine {
    double x, y, ddx, ddy, dx, dy;
    double xlen, ylen, sx, sy;
    int currentVar;
    double xold, yold;

public:
    RelaxationEngine();

    void draw();

    void search_start();
    void search_step();
};

#endif
// RELAXATION_ENGINE_H
