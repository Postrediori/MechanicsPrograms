#pragma once

class SearchEngine {
public:
    SearchEngine() = default;
    virtual ~SearchEngine() { };

    virtual void draw(CoordinateFunc /*xFunc*/, CoordinateFunc /*yFunc*/) { }
    virtual void search_start() { }
    virtual void search_step() { }

    void set_start_point(float xstart, float ystart) {
        xstart_ = xstart;
        ystart_ = ystart;
        this->search_start();
    }

    int count() const { return count_; }
    bool search_over() const { return search_over_; }
    double xmin() const { return xmin_; }
    double ymin() const { return ymin_; }
    double zmin() const { return zmin_; }

protected:
    int count_{ 0 };
    bool search_over_{ true };
    double xmin_{ 0.0 }, ymin_{ 0.0 }, zmin_{ 0.0 };
    double xstart_{ 0.0 }, ystart_{ 0.0 };
};
