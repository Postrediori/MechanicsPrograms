#include "pch.h"
#include "MathUtils.h"
#include "MediumModel.h"

MediumModel::MediumModel(float l, int n)
    : N(n), L(l) {
    InitFunc(DefUNType, DefPNType);
    InitCondL(ClosedBoundary);
    InitCondR(ClosedBoundary);

    Init();
}

void MediumModel::InitFunc(int un_type, int pn_type) {
    this->un_type = un_type;
    this->pn_type = pn_type;
}

void MediumModel::InitCondL(BoundaryCondition bc) {
    this->bl = bc.b;
    this->cl = bc.c;
}

void MediumModel::InitCondR(BoundaryCondition bc) {
    this->br = bc.b;
    this->cr = bc.c;
}

void MediumModel::Init() {
    sigma = DefSigma;

    a = DefA;
    rho = DefRho;

    Reset();
}

void MediumModel::Reset() {
    h = L / N;
    h2 = h / 2;
    tau = sigma * h / a;
    rho_a = rho * a;
    tau_h = tau / h;

    init_left = L / 3.0;
    init_right = L * 2.0 / 3.0;

    x.resize(N + 1);
    x2.resize(N);

    for (int i = 0; i <= N; i++) {
        x[i] = h * i;
    }
    for (int i = 0; i < N; i++) {
        x2[i] = x[i] + h2;
    }

    u1.resize(N);
    u2.resize(N);
    p1.resize(N);
    p2.resize(N);

    U.resize(N + 1);
    P.resize(N + 1);

    // Initial u and p values
    for (int i = 0; i < N; i++) {
        u1[i] = un(x2[i]);
        p1[i] = pn(x2[i]);
    }

    k = 0;
}

void MediumModel::Step() {
    k++;
    t = tau * k;

    float S[2][2];
    float Y[2], W[2];

    S[0][0] = 1.0;
    S[0][1] = -1.0 / rho_a;
    S[1][0] = cl;
    S[1][1] = bl;
    Y[0] = u1[0] - p1[0] / rho_a;
    Y[1] = 0.0;
    simq2(S, Y, W);
    U[0] = W[0];
    P[0] = W[1];
    /* U[0] = bl * (-p1[0] + rho_a * u1[0]) / (cl + rho_a * bl);
    P[0] = cl * ( p1[0] - rho_a * u1[0]) / (cl + rho_a * bl); */

    S[0][0] = 1.0;
    S[0][1] = 1.0 / rho_a;
    S[1][0] = cr;
    S[1][1] = br;
    Y[0] = u1[N - 1] + p1[N - 1] / rho_a;
    Y[1] = 0.0;
    simq2(S, Y, W);
    U[N] = W[0];
    P[N] = W[1];
    /* U[N] = br * ( p1[N-1] + rho_a * u1[N-1]) / (-cl + rho_a * br);
    P[N] = cr * ( p1[N-1] + rho_a * u1[N-1]) / ( cl - rho_a * br); */

    for (int i = 1; i < N; i++) {
        U[i] = ((u1[i] + u1[i - 1]) - (p1[i] - p1[i - 1]) / rho_a) / 2.0;
        P[i] = ((p1[i] + p1[i - 1]) - (u1[i] - u1[i - 1]) * rho_a) / 2.0;
    }

    for (int i = 0; i < N; i++) {
        u2[i] = u1[i] - (P[i + 1] - P[i]) * tau_h / rho;
        p2[i] = p1[i] - (U[i + 1] - U[i]) * rho_a * tau_h * a;
    }

    for (int i = 0; i < N; i++) {
        u1[i] = u2[i];
        p1[i] = p2[i];
    }
}

float MediumModel::un(float x) {
    auto un_lin = [=](float /*x*/) -> float {
        return UN_0;
    };
    auto un1 = [=](float x) -> float {
        if (x > init_left && x < init_right) return UN_0;
        else return UN_1;
    };
    auto un2 = [=](float x) -> float {
        if (x < init_left) return UN_1;
        else return UN_0;
    };
    auto un3 = [=](float x) -> float {
        if (x > init_left && x < init_right) return UN_1;
        else return UN_0;
    };
    auto un4 = [=](float x) -> float {
        if (x > init_right) return UN_1;
        else return UN_0;
    };
    auto un5 = [=](float x) -> float {
        if (x < L / 2) return -UN_1;
        else return UN_1;
    };
    auto un6 = [=](float x) -> float {
        if (x < L / 2) return UN_1;
        else return -UN_1;
    };

    switch (un_type) {
    case 0: return un_lin(x);
    case 1: return un1(x);
    case 2: return un2(x);
    case 3: return un3(x);
    case 4: return un4(x);
    case 5: return un5(x);
    case 6: return un6(x);
    default: return un_lin(x);
    }
}

float MediumModel::pn(float x) {
    auto pn_lin = [=](float /*x*/) -> float {
        return PN_0;
    };
    auto pn1 = [=](float x) -> float {
        if (x > init_left && x < init_right) return PN_0;
        else return PN_1;
    };
    auto pn2 = [=](float x) -> float {
        if (x < init_left) return PN_1;
        else return PN_0;
    };
    auto pn3 = [=](float x) -> float {
        if (x > init_left && x < init_right) return PN_1;
        else return PN_0;
    };
    auto pn4 = [=](float x) -> float {
        if (x > init_right) return PN_1;
        else return PN_0;
    };

    switch (pn_type) {
    case 0: return pn_lin(x);
    case 1: return pn1(x);
    case 2: return pn2(x);
    case 3: return pn3(x);
    case 4: return pn4(x);
    default: return pn_lin(x);
    }
}
