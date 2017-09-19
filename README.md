# Mechanics programs

This is the collection of various applications for mechanics simulations and structural engineering models. 

## Getting Started

All projects use CMake as build system. C++ code compatibility is tested
with gcc-5 and Microsoft Visual Studio Express 2008 and 2012.

## Built With

* [CMake](https://cmake.org/) - Build system.
* [FLTK](http://www.fltk.org) - Minimalistic cross-platform GUI toolkit with OpenGL capabilities.
* [GLEW](http://glew.sourceforge.net/) - Library for managing OpenGL extensions.
* [GLM](https://glm.g-truc.net/) - Mathematics and GLSL compatibility library.

## Structure

### FEMSolve

[Finite element method](https://en.wikipedia.org/wiki/Finite_element_method) solver
for 1D and 2D models with GUI. Example models are written in plain-text format 
and can be found in __models__ subdirectory.

![FEMSolve screenshot](https://user-images.githubusercontent.com/856993/30612564-afc1e4cc-9d8d-11e7-87ff-549e2d77aad6.png)
![FEMSolve report screenshot](https://user-images.githubusercontent.com/856993/30612581-c1030216-9d8d-11e7-817c-cedcadbff0d3.png)

### FinitElem

Minimalistic Finite element method solver from __FEMSolve__ in a form
of console application.

### FluidView

Graphical solver for fluid flow q and lambda functions that uses [Newton's method](https://en.wikipedia.org/wiki/Newton%27s_method).

![FluidView screenshot](https://user-images.githubusercontent.com/856993/30612458-5368f2a6-9d8d-11e7-964a-04e94a1a971f.png)

### HazeDyn

Calculator for fluid flow functions and reverse calculator
for finding lambda parameter based on function value.

![HazeDyn screenshot](https://user-images.githubusercontent.com/856993/30612418-27a2beae-9d8d-11e7-8f86-0f504db09789.png)

### MediaWave

Graphical demonstration of fluid mechanics problem of haze in a tube
of limited length with various initial boundary conditions for the tube
edges (e.g. closed, opened, with friction) as well as for pressure and velocity
of a fluid.

![MediaWave working screenshot](https://user-images.githubusercontent.com/856993/30612172-687effec-9d8c-11e7-84fc-81e7de398922.png)

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

![SimplexView screenshot](https://user-images.githubusercontent.com/856993/30611865-689abeea-9d8b-11e7-8523-39688b65cc65.png)

### WaveView

Graphical demonstration of fluid mechanics problem of a fluid in
rectangular channel of infinite length.

![WaveView screenshot](https://user-images.githubusercontent.com/856993/30611938-af73b5ba-9d8b-11e7-86fc-0271d2dcd48e.png)
