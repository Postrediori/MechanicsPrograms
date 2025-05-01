#pragma once

#define DRAW_METHOD_OPENGL 1
#define DRAW_METHOD_FLTK 2
#ifndef DRAW_METHOD
#define DRAW_METHOD DRAW_METHOD_FLTK
#endif

using CoordinateFunc = std::function<float(float)>;

void DrawLine(float x1, float y1, float x2, float y2);
void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
void DrawRectangle(float x1, float y1, float x2, float y2);
void DrawArrow(float x1, float y1, float x2, float y2);

#if DRAW_METHOD==DRAW_METHOD_OPENGL
void PrintText(void *font, float x, float y, const char *fmt, ...);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
void PrintText(int font, float x, float y, const char* fmt, ...);
#endif
