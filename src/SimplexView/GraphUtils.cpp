#include "pch.h"
#include "GraphUtils.h"

void PrintText(void *font, double x, double y, TextAlign align, const char *fmt, ...) {
    constexpr size_t BufferLength = 256;
    char text[BufferLength] = { 0 };
    va_list ap;

    if (fmt == NULL) {
        return;
    }

    va_start(ap, fmt);
    vsnprintf(text, BufferLength - 1, fmt, ap);
    va_end(ap);

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glRasterPos2d(x, y);
    glutBitmapString(font, reinterpret_cast<const unsigned char*>(text));
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_font(FL_HELVETICA, static_cast<int>(reinterpret_cast<uintptr_t>(font)));

    int w{ 0 }, h{ 0 };
    fl_measure(text, w, h);

    double x0{ x }, y0{ y };

    if (align & HorizontalTextAlign::Left) {
        x0 -= w;
    }
    else if (align & HorizontalTextAlign::Center) {
        x0 -= static_cast<double>(w) / 2.0;
    }

    if (align & VerticalTextAlign::Bottom) {
        y0 += h;
    }
    else if (align & VerticalTextAlign::Middle) {
        y0 += static_cast<double>(h) / 2.0;
    }

    fl_draw(text, x0, y0);
#endif
}

void DrawLine(float x1, float y1, float x2, float y2) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_line(x1, y1, x2, y2);
#endif
}

void DrawRectangle(float x1, float y1, float x2, float y2) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_polygon(x1, y1,
        x2, y1,
        x2, y2,
        x1, y2);
#endif
}
