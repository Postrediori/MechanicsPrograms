// GaussSEngine.h
#ifndef GAUSSS_ENGINE_H
#define GAUSSS_ENGINE_H

#include "SearchEngine.h"

class GaussSEngine: public SearchEngine {
    int currentVar;
    double ddx, ddy;
    double x, y;
    double xold, yold;

public:
    GaussSEngine();

    void draw();

    void search_start();
    void search_step();
};

#endif
// GAUSSS_ENGINE_H
