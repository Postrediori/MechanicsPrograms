#include "pch.h"
#include "GeometryUtils.h"
#include "OptimizationModel.h"


/******************************************************************************
 * General functions for optimization models
 *****************************************************************************/

void Model::init_search(Area functionArea, double e, Function1D f) {
    search_ = false;
    counter_ = 0;

    area_ = functionArea;
    min_ = area_.xmin;
    max_ = area_.xmax;
    e_ = e;
    f_ = f;
}

void Model::reset() {
    x1_ = x2_ = 0.0;
    y1_ = y2_ = 0.0;
}


void Model::draw(CoordinateFunction coordFunc) const {
    // Draw current minimum
    fl_color(fl_rgb_color(0, 64, 192));
    fl_line_style(FL_SOLID, 1);

    auto p1 = coordFunc({ x1_, y1_ });
    auto p2 = coordFunc({ x1_, area_.ymin });
    fl_line(p1.x, p1.y,
        p2.x, p2.y);

    // Draw search status
    fl_color(fl_rgb_color(32, 128, 32));
    fl_line_style(FL_SOLID, 1);

    auto p3 = coordFunc({ x2_, y2_ });
    auto p4 = coordFunc({ x2_, area_.ymin });
    fl_line(p3.x, p3.y,
        p4.x, p4.y);

    // Draw marker for current search min
    constexpr int MarkerSize = 10;
    fl_color(fl_rgb_color(0, 64, 192));
    fl_rectf(p1.x - MarkerSize / 2, p1.y - MarkerSize / 2,
        MarkerSize, MarkerSize);
}


/******************************************************************************
 * Model for scaning method
 *****************************************************************************/

void ScanModel::start() {
    search_ = true;
    counter_ = 0;

    double len = max_ - min_;
    int N = 2 * len / e_ - 1;
    dx_ = len / (double)(N);

    x1_ = x2_ = min_;
    y1_ = y2_ = f_(min_);
    counter_++;
}

void ScanModel::step() {
    if (!search_) {
        return;
    }

    x2_ += dx_;
    if (x2_>max_) {
        x2_ = max_;
        stop();
    }

    y2_ = f_(x2_);
    counter_++;

    if (y2_<y1_) {
        x1_ = x2_;
        y1_ = y2_;
    }
}


/******************************************************************************
 * Model for Fibonacci method
 *****************************************************************************/

void FibModel::start() {
    search_ = true;
    counter_ = 0;

    double len = max_ - min_;
    int N = int(len / e_);

    int k = 2;
    while (N>fib_[k-1]) {
        fib_[k] = fib_[k-1] + fib_[k-2];
        k++;
    }

    k_ = k - 1;
    dm_ = len / double(fib_[k_]);

    x1_ = x2_ = min_;
    y1_ = y2_ = f_(min_);
    k_ -= 2;
    counter_++;
}

void FibModel::step() {
    if (!search_) {
        return;
    }

    if (k_<=1) {
        stop();
        return;
    }

    x1_ = x2_;
    y1_ = y2_;

    double x1 = x2_+dm_*fib_[k_];
    double y1 = f_(x1), y2 = f_(x2_);

    if (y1 < y2) {
        x2_ = x1 + dm_ * fib_[k_ - 1];
    }
    else {
        x2_ -= dm_ * fib_[k_ - 1];
    }

    y1_ = y2;
    k_ -= 2;

    y2_ = f_(x2_);
    counter_ += 3;
}


/******************************************************************************
 * Model for Gradient method
 *****************************************************************************/

void GradModel::start() {
    search_ = true;
    counter_ = 0;

    dx_ = 100.0 * e_;

    x1_ = min_;
    y1_ = f_(x1_);

    x2_ = min_ + dx_;
    y2_ = f_(x2_);

    counter_ += 2;
}

void GradModel::step() {
    if (!search_) {
        return;
    }

    if (y1_<=y2_) {
        stop();
        return;
    }

    double x = x2_-dx_*(y2_-y1_)/(x2_-x1_);

    if (x>max_) {
        x1_ = max_;
        y1_ = f_(x1_);
        counter_++;

        stop();
        return;
    }

    x1_ = x2_; y1_ = y2_;
    x2_ = x; y2_ = f_(x);
    counter_++;
}


/******************************************************************************
 * Model for Bounce method
 *****************************************************************************/

void BounceModel::start() {
    search_ = true;
    counter_ = 0;

    dx_ = 100.0 * e_;

    x1_ = min_;
    y1_ = f_(x1_);

    x2_ = min_ + dx_;
    y2_ = f_(x2_);

    counter_ += 2;
}

void BounceModel::step() {
    if (!search_) {
        return;
    }

    if (y1_<=y2_) {
        stop();
        return;
    }

    double x = x2_-dx_*(y2_-y1_)/(x2_-x1_) + dx_ * (x2_ - x1_);

    if (x>max_) {
        x1_ = max_;
        y1_ = f_(x1_);
        counter_++;

        stop();
        return;
    }

    x1_ = x2_; y1_ = y2_;
    x2_ = x; y2_ = f_(x);
    counter_++;
}


/******************************************************************************
 * Model for Boltzano method
 *****************************************************************************/

void BolzanoModel::start() {
    search_ = true;
    counter_ = 0;

    a_ = min_;
    b_ = max_;
    l_ = (b_ - a_) / 2.0;
}

void BolzanoModel::step(){
    if (!search_) {
        return;
    }

    if (l_<=e_) {
        stop();
        return;
    }

    x1_ = (a_ + b_) / 2.0;
    if (f_(x1_+e_)>=f_(x1_-e_)) {
        b_ = x1_;
        a_ = a_;
    }
    else {
        a_ = x1_;
        b_ = b_;
    }
    l_ = (b_ - a_) / 2.0;

    x2_ = x1_;
    y2_ = y1_ = f_(x1_);

    counter_ += 3;
}
