#include "pch.h"
#include "GraphicsUtils.h"

#if DRAW_METHOD==DRAW_METHOD_OPENGL
void PrintText(void* font, float x, float y, const char* fmt, ...) {
#elif DRAW_METHOD==DRAW_METHOD_FLTK
void PrintText(int font, float x, float y, const char* fmt, ...) {
#endif
    const size_t BufferLen = 256;
    char text[BufferLen] = { 0 };
    va_list ap;

    if (!fmt) {
        return;
    }

    va_start(ap, fmt);
    vsnprintf(text, BufferLen - 1, fmt, ap);
    va_end(ap);

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    int h = glutBitmapHeight(font);
    int w = 0;
    char* p = text;
    while (*p != '\0') {
        w += glutBitmapWidth(font, *p);
        p++;
    }
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    int w{ 0 }, h{ 0 };
    fl_measure(text, w, h);
#endif

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glRasterPos2f(x, y);
    glutBitmapString(font, reinterpret_cast<const unsigned char *>(text));
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_draw(text, x, y);
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

void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_polygon(x1, y1,
        x2, y2,
        x3, y3);
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

void DrawArrow(float x1, float y1, float x2, float y2) {
    constexpr float ArrowSize = 0.2;
    constexpr float Rad2Deg = 180.0 / M_PI;

    float dx = x2 - x1;
    float dy = y2 - y1;
    float angle = atan2(dy, dx);
    float len = sqrt(dx * dx + dy * dy);
    float arrow_size = len * ArrowSize;

#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glPushMatrix();

    glTranslatef(x2, y2, 0.0);
    glRotatef(angle * Rad2Deg, 0.0, 0.0, 1.0);

    glBegin(GL_LINES);
    glVertex2f(-len, 0.0);
    glVertex2f(0.0, 0.0);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(0.0, 0.0);
    glVertex2f(-arrow_size, arrow_size);
    glVertex2f(-arrow_size, -arrow_size);
    glEnd();

    glPopMatrix();
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_line(x1, y1, x2, y2);

    fl_line(x2, y2,
        x2 + arrow_size * cos(-angle + M_PI / 6), y2 + arrow_size * sin(-angle + M_PI / 6));

    fl_line(x2, y2,
        x2 + arrow_size * cos(-angle - M_PI / 6), y2 + arrow_size * sin(-angle - M_PI / 6));
#endif
}
