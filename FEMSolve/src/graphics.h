// graphics.h
#ifndef GRAPHICS_H
#define GRAPHICS_H

void DrawLine(float x1, float y1, float x2, float y2);
void DrawRectangle(float x1, float y1, float x2, float y2);
void DrawArrow(float x1, float y1, float x2, float y2);

void PrintText(void *font, const char *fmt, ...);

#endif
// GRAPHICS_H
