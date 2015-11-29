// SearchEngine.h
#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

class SearchEngine {
protected:
    int count_;
    bool search_over_;
    double xmin_, ymin_, zmin_;
    double xstart_, ystart_;

public:
    SearchEngine() { }

    virtual void draw() { }
    virtual void search_start() { }
    virtual void search_step() { }
    void set_start_point(float xstart, float ystart) {
        xstart_ = xstart;
        ystart_ = ystart;
        this->search_start();
    }

    int count() { return count_; }
    bool search_over() { return search_over_; }
    double xmin() { return xmin_; }
    double ymin() { return ymin_; }
    double zmin() { return zmin_; }
};

#endif
// SEARCH_ENGINE_H
