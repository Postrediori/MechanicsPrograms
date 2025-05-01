#pragma once

#include <FL/Fl.H>
#if DRAW_METHOD==DRAW_METHOD_OPENGL
#include <FL/Fl_Gl_Window.H>
#endif
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Round_Button.H>

#include <FL/gl.h>
#include <FL/glu.h>
#include <FL/glut.H>

#include <FL/x.H>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdarg>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include <HandmadeMath.h>
