#pragma once

// Initial conditions
constexpr float DefL = 10.0;
constexpr int DefN = 100;
constexpr float DefA = 1.0;
constexpr float DefRho = 1.0;
constexpr float DefSigma = 0.1;

struct BoundaryCondition {
    float b{ 0.0f }, c{ 0.0f };
};

const BoundaryCondition ClosedBoundary = {0.0f, 1.0f};
const BoundaryCondition OpenBoundary = {1.0f, 0.0f};

constexpr int DefUNType = 0;
constexpr int DefPNType = 2;

// u(x) initial conditions
constexpr float UN_0 = 0.0;
constexpr float UN_1 = 1.0;

// p(x) initial conditions
constexpr float PN_0 = 0.0;
constexpr float PN_1 = 1.0;

struct MediumModel {
    MediumModel(float l, int n);

    void Init();
    void Reset();
    void Step();

    void InitFunc(int un_type, int pn_type);
    void InitCondL(BoundaryCondition bc);
    void InitCondR(BoundaryCondition bc);

    float un(float x);
    float pn(float x);

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

    std::vector<float> x, x2;
    std::vector<float> u1, u2, p1, p2;
    std::vector<float> U, P;
};
