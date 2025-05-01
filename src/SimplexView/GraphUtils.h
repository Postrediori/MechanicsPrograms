#pragma once

#define DRAW_METHOD_OPENGL 1
#define DRAW_METHOD_FLTK 2
#ifndef DRAW_METHOD
#define DRAW_METHOD DRAW_METHOD_FLTK
#endif

#if DRAW_METHOD==DRAW_METHOD_OPENGL
#define SET_FL_COLOR_TO_GL(c) { \
    GLubyte r, g, b, a; \
    Fl::get_color((c), r, g, b, a); \
    glColor4ub(r, g, b, a); \
    }
#endif

using CoordinateFunc = std::function<double(double)>;

using TextAlign = uint8_t;

namespace VerticalTextAlign {
    const TextAlign Top = 0x01;
    const TextAlign Middle = 0x02;
    const TextAlign Bottom = 0x04;
}

namespace HorizontalTextAlign {
    const TextAlign Left = 0x10;
    const TextAlign Center = 0x20;
    const TextAlign Right = 0x40;
}

void PrintText(void *font, double x, double y, TextAlign align, const char *fmt, ...);

void DrawLine(float x1, float y1, float x2, float y2);
void DrawRectangle(float x1, float y1, float x2, float y2);
