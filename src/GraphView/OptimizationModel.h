#pragma once

constexpr double DefaultTimerInterval = 0.025;

/******************************************************************************
 * Base model class
 *****************************************************************************/
class Model {
public:
    Model() = default;
    virtual ~Model() { }

    virtual void start() { }
    virtual void step() { }
    virtual double timer_interval() const { return DefaultTimerInterval; }

    // Name of the model displayed in the program UI
    virtual const char* name() const { return "Unimplemented"; }

    // Draw current model status using coordFunc as a function
    // to transfrom from model coordinate system to screen coordinates
    virtual void draw(CoordinateFunction coordFunc) const;

    void stop() { search_ = false; }

    bool search() const { return search_; }
    int counter() const { return counter_; }

    double x1() const { return x1_; }
    double y1() const { return y1_; }

    void init_search(Area functionArea, double e, Function1D f);
    void reset();

protected:
    bool search_ = false;
    int counter_ = 0;

    Area area_ = {-1.0, 1.0, -1.0, 1.0};
    double min_ = 0.0, max_ = 0.0;
    double e_ = 0.0;
    Function1D f_;

    double x1_ = 0.0, x2_ = 0.0;
    double y1_ = 0.0, y2_ = 0.0;
};

/******************************************************************************
 * Model for scaning method
 *****************************************************************************/
class ScanModel: public Model {
public:
    ScanModel() = default;
    void start();
    void step();

    double timer_interval() const { return 0.0025; }
    const char* name() const override { return "Scanning"; }

protected:
    double dx_ = 0.0;
};

/******************************************************************************
 * Model for Fibonacci method
 *****************************************************************************/
class FibModel: public Model {
public:
    FibModel() = default;

    void start();
    void step();
    double timer_interval() const { return 1.0; }
    const char* name() const override { return "Fibonacci"; }

protected:
    int fib_[50] = { 1 };
    int k_ = 0;

    double dm_ = 0.0;
};

/******************************************************************************
 * Model for Gradient method
 *****************************************************************************/
class GradModel: public Model {
public:
    GradModel() = default;

    void start();
    void step();
    double timer_interval() const { return 0.1; }
    const char* name() const override { return "Gradient descent"; }

protected:
    double dx_ = 0.0;
};

/******************************************************************************
 * Model for Bounce method
 *****************************************************************************/
class BounceModel: public Model {
public:
    BounceModel() = default;

    void start();
    void step();
    double timer_interval() const { return 0.1; }
    const char* name() const override { return "Heavy-ball method"; }

protected:
    double dx_ = 0.0;
};

/******************************************************************************
 * Model for Boltzano method
 *****************************************************************************/
class BolzanoModel: public Model {
public:
    BolzanoModel() = default;

    void start();
    void step();
    double timer_interval() const { return 0.5; }
    const char* name() const override { return "Bolzano"; }

protected:
    double l_ = 0.0;
    double a_ = 0.0, b_ = 0.0;
};
