// graphics.cpp

#include <stdarg.h>
#include <math.h>
#include <FL/gl.h>
#include <FL/glut.H>

void PrintText(void* font, const char* fmt, ... ) {
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

void DrawArrow(float x1, float y1, float x2, float y2) {
    const float ArrowSize = 0.2;
    const float Pi = 3.14159265;
    const float Rad2Deg = 180.0 / Pi;

    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrt(dx*dx+dy*dy);
    float angle = atan2(dy, dx) * Rad2Deg;
    float arrow_size_x = len * ArrowSize;
    float arrow_size_y = len * ArrowSize;

    glPushMatrix();

    glTranslatef(x2, y2, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);

    glBegin(GL_LINES);
    glVertex2f(-len, 0.0);
    glVertex2f(0.0, 0.0);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(0.0, 0.0);
    glVertex2f(-arrow_size_x, arrow_size_y);
    glVertex2f(-arrow_size_x, -arrow_size_y);
    glEnd();

    glPopMatrix();
}
