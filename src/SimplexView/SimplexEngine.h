// SimplexEngine.h
#ifndef SIMPLEX_ENGINE_H
#define SIMPLEX_ENGINE_H

#include <vector>
#include "Func.h"
#include "Simplex.h"
#include "SearchEngine.h"

class SimplexEngine: public SearchEngine {
    SIMPLEX simplex_;
    std::vector<SIMPLEX> history_;
    
    void update_min();

public:
    SimplexEngine();

    void draw();

    void search_start();
    void search_step();
};

#endif
// SIMPLEX_ENGINE_H
