#pragma once

static const int g_modelDimensions = 2;

struct NODE {
    int node;
    double x, y;
};

struct FIX {
    int node;
    int axis;
    // int fx, fy;
};

struct LOAD {
    int node;
    double px, py;
};

struct ELEM {
    int nodes[g_modelDimensions];
    double length;
    double sina, cosa;
};

class Model {
public:
    Model();
    
    bool LoadFromFile(const std::string& fileName);
    void PrepareSolve();
    void Solve();
        
public:
    int component_count_;
    
    double E_, F_;
    
    std::vector<NODE> nodes_;
    std::vector<FIX> fixes_;
    std::vector<LOAD> loads_;
    std::vector<ELEM> elements_;
    
    arma::mat a_;
    arma::vec b_, u_, n_;
};
