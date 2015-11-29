// Graph.cpp

#include <FL/glut.H>
#include <stdarg.h>

void PrintText(void *font, const char *fmt, ...) {
    char text[256];
    va_list ap;

    if (fmt==NULL) return;

    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);

    char *p = (char *) text;
    while (*p!='\0') {
        glutBitmapCharacter(font, *p);
        p++;
    }
}

void DrawLine(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void DrawRectangle(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}
