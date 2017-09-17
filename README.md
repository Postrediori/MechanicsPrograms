# Mechanics programs

This is the collection of various applications for mechanics simulations and structural engineering models. 

## Getting Started

All projects use CMake as build system. C++ code compatibility is tested
with gcc-5 and Microsoft Visual Studio Express 2008 and 2012.

## Build With

* [CMake](https://cmake.org/) - Build system.
* [FLTK](http://www.fltk.org) - Minimalistic cross-platform GUI toolkit with OpenGL capabilities.
* [GLEW](http://glew.sourceforge.net/) - Library for managing OpenGL extensions.
* [GLM](https://glm.g-truc.net/) - Mathematics and GLSL compatibility library.

## Structure

### FEMSolve

[Finite element method](https://en.wikipedia.org/wiki/Finite_element_method) solver
for 1D and 2D models with GUI. Example models are written in plain-text format 
and can be found in __models__ subdirectory.

![FEMSolve screenshot](https://user-images.githubusercontent.com/856993/30521525-9c445c56-9bc9-11e7-819f-b99dfec19698.png)
![FEMSolve report screenshot](https://user-images.githubusercontent.com/856993/30521518-8a315a6e-9bc9-11e7-9b41-acc4b0be3870.png)

### FinitElem

Minimalistic Finite element method solver from __FEMSolve__ in a form
of console application.

### FluidView

Graphical solver for fluid flow q and lambda functions that uses [Newton's method](https://en.wikipedia.org/wiki/Newton%27s_method).

![FluidView screenshot](https://user-images.githubusercontent.com/856993/30521531-acc913c8-9bc9-11e7-8aa6-bd545eb10451.png)

### HazeDyn

Calculator for fluid flow functions and reverse calculator
for finding lambda parameter based on function value.

![HazeDyn screenshot](https://user-images.githubusercontent.com/856993/30521536-c23f1b3a-9bc9-11e7-9ac1-640fb8c0d194.png)

### MediaWave

Graphical demonstration of fluid mechanics problem of haze in a tube
of limited length with various initial boundary conditions for the tube
edges (e.g. closed, opened, with friction) as well as for pressure and velocity
of a fluid.

![MediaWave settings screenshot](https://user-images.githubusercontent.com/856993/30521540-d6a3ca44-9bc9-11e7-9c84-5525d27deb51.png)
![MediaWave working screenshot](https://user-images.githubusercontent.com/856993/30521538-cc4fb12a-9bc9-11e7-932e-f45c42208297.png)

### SimplexView

GUI tool originally designed for demonstration of Nelder–Mead optimization
method using triangle simplex for functions with two parameters (hence the name).
Later several other methods of function minimizaion were added. Now the following
methods are supported with graphical demonstration for each of them:

* [Nelder–Mead method](https://en.wikipedia.org/wiki/Nelder%E2%80%93Mead_method) with triangle as simplex polygon.
* [Gauss-Seidel method](https://en.wikipedia.org/wiki/Gauss%E2%80%93Seidel_method).
* [Gradient descent](https://en.wikipedia.org/wiki/Gradient_descent).
* [Coordinate descent](https://en.wikipedia.org/wiki/Coordinate_descent).
* Scanning method.

![SimplexView screenshot](https://user-images.githubusercontent.com/856993/30521543-e1e6553e-9bc9-11e7-9fa9-a0540d3433e5.png)

### WaveView

Graphical demonstration of fluid mechanics problem of a fluid in
rectangular channel of infinite length.

![WaveView screenshot](https://user-images.githubusercontent.com/856993/30521548-eb2eb87a-9bc9-11e7-836f-047dcc7fe631.png)
