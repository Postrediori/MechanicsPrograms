#pragma once

using SurfaceFunc = std::function<double(double x, double delta, double epsilon)>;

double surface_func_linear(double x, double delta, double epsilon);
double surface_func_sine(double x, double delta, double epsilon);
double surface_func_cosine(double x, double delta, double epsilon);
double surface_func_polygonal(double x, double delta, double epsilon);
