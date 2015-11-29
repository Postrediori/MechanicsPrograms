// ScanEngine.h
#ifndef SCAN_ENGINE_H
#define SCAN_ENGINE_H

#include "SearchEngine.h"

class ScanEngine: public SearchEngine {
    double dx, dy;

public:
    ScanEngine();

    void draw();

    void search_start();
    void search_step();
};

#endif
// SCAN_ENGINE_H
