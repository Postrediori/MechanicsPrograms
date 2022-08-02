#pragma once

// Initial conditions
static const float DefL = 10.0;
static const int DefN = 100;
static const float DefA = 1.0;
static const float DefRho = 1.0;
static const float DefSigma = 0.1;

#define INIT_COND_CLOSED 0.0,1.0
#define INIT_COND_OPENED 1.0,0.0
#define INIT_COND(b,c) b,c

static const int DefUNType = 0;
static const int DefPNType = 0;

// u(x) initial conditions
const float UN_0 = 0.0;
const float UN_1 = 1.0;

// p(x) initial conditions
const float PN_0 = 0.0;
const float PN_1 = 1.0;

class MediumModel {
public:
    int N;
    float L;
    float sigma, a;
    float h, h2;
    float rho, rho_a;
    float t, tau, tau_h;

    int k;

    float bl, cl;
    float br, cr;

    float init_left, init_right;
    int un_type, pn_type;

    float un(float);
    float un_lin(float);
    float un1(float);
    float un2(float);
    float un3(float);
    float un4(float);
    float un5(float);
    float un6(float);

    float pn(float);
    float pn_lin(float);
    float pn1(float);
    float pn2(float);
    float pn3(float);
    float pn4(float);

    std::vector<float> x, x2;
    std::vector<float> u1, u2, p1, p2;
    std::vector<float> U, P;

public:
    MediumModel(float, int);

    void Init();
    void Reset();
    void Step();

    void InitFunc(int, int);
    void InitCondL(float, float);
    void InitCondR(float, float);
};
