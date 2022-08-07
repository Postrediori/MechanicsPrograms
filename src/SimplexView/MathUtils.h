#pragma once

struct vec2 {
    float x{ 0.0f }, y{ 0.0f };

    vec2() = default;
    vec2(float X, float Y) : x(X), y(Y) { }
};

struct vec4 {
    float x{ 0.0f }, y{ 0.0f }, z{ 0.0f }, w{ 0.0f };

    vec4() = default;
    vec4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) { }
};

constexpr double Epsilon = 1e-2;

int signum(double x);

bool almost_equal(double a, double b);

