// model.h
#ifndef MODEL_H
#define MODEL_H

static const int MODEL_DIMENSIONS = 2;

struct NODE {
    int node;
    float x, y;
};

struct FIX {
    int node;
    int axis;
    // int fx, fy;
};

struct LOAD {
    int node;
    float px, py;
};

struct ELEM {
    int nodes[MODEL_DIMENSIONS];
    float length;
    float sina, cosa;
};

class FinitModel {
public:
    int component_count;
    int node_count;
    NODE *nodes;

    int fixed_count;
    FIX *fixed;

    int load_count;
    LOAD *loads;

    int elem_count;
    ELEM *elems;

    float **a;
    float *b;
    float *u;
    float *n;

    float E, F;

public:
    FinitModel();
    ~FinitModel();

    void load_from_file(const char *name);
    void clear();

    void prepare_solve();
    void solve();
};

#endif
// MODEL_H
