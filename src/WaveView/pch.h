#pragma once

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/x.H>
#include <FL/gl.h>
#include <FL/glu.h>
#include <FL/glut.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#if DRAW_METHOD==DRAW_METHOD_OPENGL
#include <FL/Fl_Gl_Window.H>
#endif
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>

#include <array>
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include <stb_image_write.h>
