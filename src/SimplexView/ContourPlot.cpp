// ContourPlot.cpp

#include "ContourPlot.h"
#include <cmath>
#include <FL/gl.h>

#define TYPE_NORTH_WEST 0x01
#define TYPE_NORTH_EAST 0x02
#define TYPE_SOUTH_EAST 0x04
#define TYPE_SOUTH_WEST 0x08

static int GetCellType(float vals[]) {
    int type = 0;
    if (vals[0]>0.0f) type |= TYPE_NORTH_WEST;
    if (vals[1]>0.0f) type |= TYPE_NORTH_EAST;
    if (vals[2]>0.0f) type |= TYPE_SOUTH_EAST;
    if (vals[3]>0.0f) type |= TYPE_SOUTH_WEST;
    return type;
}

void CreateCorner(std::vector<vec2>& lines, int type,
                float x, float y, float sx, float sy, float vals[]) {
    float x1, y1;
    float x2, y2;

    switch (type) {
    case 1:
    case 14:
        x1 = x;
        y1 = y+sy*fabs(vals[0]/(vals[0]-vals[3]));
        x2 = x+sx*fabs(vals[0]/(vals[0]-vals[1]));
        y2 = y;
        break;
    case 2:
    case 13:
        x1 = x+sx*fabs(vals[0]/(vals[0]-vals[1]));
        y1 = y;
        x2 = x+sx;
        y2 = y+sy*fabs(vals[1]/(vals[1]-vals[2]));
        break;
    case 4:
    case 11:
        x1 = x+sx*fabs(vals[3]/(vals[3]-vals[2]));
        y1 = y+sy;
        x2 = x+sx;
        y2 = y+sy*fabs(vals[1]/(vals[1]-vals[2]));
        break;
    case 7:
    case 8:
        x1 = x;
        y1 = y+sy*fabs(vals[0]/(vals[0]-vals[3]));
        x2 = x+sx*fabs(vals[3]/(vals[3]-vals[2]));
        y2 = y+sy;
        break;
    }

    lines.push_back(vec2(x1, y1));
    lines.push_back(vec2(x2, y2));
}

void CreateHalf(std::vector<vec2>& lines, int type,
              float x, float y, float sx, float sy, float vals[]) {
    float x1, y1;
    float x2, y2;

    switch (type) {
    case 3:
    case 12:
        x1 = x;
        y1 = y+sy*fabs(vals[0]/(vals[0]-vals[3]));
        x2 = x+sx;
        y2 = y+sy*fabs(vals[1]/(vals[1]-vals[2]));
        break;
    case 6:
    case 9:
        x1 = x+sx*fabs(vals[0]/(vals[0]-vals[1]));
        y1 = y;
        x2 = x+sx*fabs(vals[3]/(vals[3]-vals[2]));
        y2 = y+sy;
        break;
    }

    lines.push_back(vec2(x1, y1));
    lines.push_back(vec2(x2, y2));
}

void CreateAmbiguity(std::vector<vec2>& lines, int flags,
                   float x, float y, float sx, float sy, float vals[], float /*v*/,
                   bool u) {
    float x1, y1;
    float x2, y2;
    float x3, y3;
    float x4, y4;

    if ((flags==5 && u) || (flags==10 && !u)) {
        x1 = x;
        y1 = y+sy*fabs(vals[0]/(vals[0]-vals[3]));
        x2 = x+sx*fabs(vals[3]/(vals[3]-vals[2]));
        y2 = y+sy;

        x3 = x+sx*fabs(vals[0]/(vals[0]-vals[1]));
        y3 = y;
        x4 = x+sx;
        y4 = y+sy*fabs(vals[1]/(vals[1]-vals[2]));
    }

    if ((flags==5 && !u) || (flags==10 && u)) {
        x1 = x;
        y1 = y+sy*fabs(vals[0]/(vals[0]-vals[3]));
        x2 = x+sx*fabs(vals[0]/(vals[0]-vals[1]));
        y2 = y;

        x3 = x+sx*fabs(vals[3]/(vals[3]-vals[2]));
        y3 = y+sy;
        x4 = x+sx;
        y4 = y+sy*fabs(vals[1]/(vals[1]-vals[2]));
    }

    lines.push_back(vec2(x1, y1));
    lines.push_back(vec2(x2, y2));
    lines.push_back(vec2(x3, y3));
    lines.push_back(vec2(x4, y4));
}

void CreateFullFilled(std::vector<vec2>& triangles,
              float x, float y, float sx, float sy) {
    triangles.push_back(vec2(x,    y));
    triangles.push_back(vec2(x+sx, y+sy));
    triangles.push_back(vec2(x,    y+sy));

    triangles.push_back(vec2(x,    y));
    triangles.push_back(vec2(x+sx, y));
    triangles.push_back(vec2(x+sx, y+sy));
}

void CreateFilledCorner(std::vector<vec2>& triangles, int type,
                    float x, float y, float sx, float sy, float vals[]) {
    float x1, y1;
    float x2, y2;

    switch (type) {
    case 1:
    case 14:
        x1 = x+sx*fabs(vals[0]/(vals[0]-vals[1]));
        y1 = y;
        x2 = x;
        y2 = y+sy*fabs(vals[0]/(vals[0]-vals[3]));
        break;

    case 2:
    case 13:
        x1 = x+sx;
        y1 = y+sy*fabs(vals[1]/(vals[1]-vals[2]));
        x2 = x+sx*fabs(vals[0]/(vals[0]-vals[1]));
        y2 = y;
        break;

    case 4:
    case 11:
        x1 = x+sx*fabs(vals[3]/(vals[3]-vals[2]));
        y1 = y+sy;
        x2 = x+sx;
        y2 = y+sy*fabs(vals[1]/(vals[1]-vals[2]));
        break;

    case 7:
    case 8:
        x1 = x;
        y1 = y+sy*fabs(vals[0]/(vals[0]-vals[3]));
        x2 = x+sx*fabs(vals[3]/(vals[3]-vals[2]));
        y2 = y+sy;
        break;
    }

    if (type==1) {
        triangles.push_back(vec2(x, y));
        triangles.push_back(vec2(x1, y1));
        triangles.push_back(vec2(x2, y2));

    } else if (type==14) {
        triangles.push_back(vec2(x1, y1));
        triangles.push_back(vec2(x+sx, y));
        triangles.push_back(vec2(x+sx, y+sy));

        triangles.push_back(vec2(x1, y1));
        triangles.push_back(vec2(x+sx, y+sy));
        triangles.push_back(vec2(x2, y2));

        triangles.push_back(vec2(x2, y2));
        triangles.push_back(vec2(x+sx, y+sy));
        triangles.push_back(vec2(x, y+sy));

    } else if (type==2) {
        triangles.push_back(vec2(x+sx, y));
        triangles.push_back(vec2(x1, y1));
        triangles.push_back(vec2(x2, y2));

    } else if (type==13) {
        triangles.push_back(vec2(x, y));
        triangles.push_back(vec2(x2, y2));
        triangles.push_back(vec2(x, y+sy));

        triangles.push_back(vec2(x, y+sy));
        triangles.push_back(vec2(x2, y2));
        triangles.push_back(vec2(x1, y1));

        triangles.push_back(vec2(x, y+sy));
        triangles.push_back(vec2(x1, y1));
        triangles.push_back(vec2(x+sx, y+sy));

    } else if (type==4) {
        triangles.push_back(vec2(x+sx, y+sy));
        triangles.push_back(vec2(x1, y1));
        triangles.push_back(vec2(x2, y2));

    } else if (type==11) {
        triangles.push_back(vec2(x, y));
        triangles.push_back(vec2(x+sx, y));
        triangles.push_back(vec2(x2, y2));

        triangles.push_back(vec2(x, y));
        triangles.push_back(vec2(x2, y2));
        triangles.push_back(vec2(x1, y1));

        triangles.push_back(vec2(x, y));
        triangles.push_back(vec2(x1, y1));
        triangles.push_back(vec2(x, y+sy));

    } else if (type==8) {
        triangles.push_back(vec2(x, y+sy));
        triangles.push_back(vec2(x1, y1));
        triangles.push_back(vec2(x2, y2));

    } else if (type==7) {
        triangles.push_back(vec2(x, y));
        triangles.push_back(vec2(x+sx, y));
        triangles.push_back(vec2(x1, y1));

        triangles.push_back(vec2(x1, y1));
        triangles.push_back(vec2(x+sx, y));
        triangles.push_back(vec2(x2, y2));

        triangles.push_back(vec2(x+sx, y));
        triangles.push_back(vec2(x+sx, y+sy));
        triangles.push_back(vec2(x2, y2));
    }
}

void CreateFilledHalf(std::vector<vec2>& triangles, int type,
                  float x, float y, float sx, float sy, float vals[]) {
    float x1, y1;
    float x2, y2;
    float x3, y3;
    float x4, y4;

    switch (type) {
    case 3:
        x1 = x;
        y1 = y;
        x2 = x+sx;
        y2 = y;
        x3 = x+sx;
        y3 = y+sy*fabs(vals[1]/(vals[1]-vals[2]));
        x4 = x;
        y4 = y+sy*fabs(vals[0]/(vals[0]-vals[3]));
        break;

    case 12:
        x1 = x;
        y1 = y+sy*fabs(vals[0]/(vals[0]-vals[3]));
        x2 = x+sx;
        y2 = y+sy*fabs(vals[1]/(vals[1]-vals[2]));
        x3 = x+sx;
        y3 = y+sy;
        x4 = x;
        y4 = y+sy;
        break;

    case 6:
        x1 = x+sx*fabs(vals[0]/(vals[0]-vals[1]));
        y1 = y;
        x2 = x+sx;
        y2 = y;
        x3 = x+sx;
        y3 = y+sy;
        x4 = x+sx*fabs(vals[3]/(vals[3]-vals[2]));
        y4 = y+sy;
        break;

    case 9:
        x1 = x;
        y1 = y;
        x2 = x+sx*fabs(vals[0]/(vals[0]-vals[1]));
        y2 = y;
        x3 = x+sx*fabs(vals[3]/(vals[3]-vals[2]));
        y3 = y+sy;
        x4 = x;
        y4 = y+sy;
        break;
    }

    triangles.push_back(vec2(x1, y1));
    triangles.push_back(vec2(x3, y3));
    triangles.push_back(vec2(x4, y4));

    triangles.push_back(vec2(x1, y1));
    triangles.push_back(vec2(x2, y2));
    triangles.push_back(vec2(x3, y3));
}

void CreateFilledAmbiguity(std::vector<vec2>& triangles, int type,
                       float x, float y, float sx, float sy, float vals[],
                       float /*v*/, bool u) {
    float x1, y1;
    float x2, y2;
    float x3, y3;
    float x4, y4;

    x1 = x+sx*fabs(vals[0]/(vals[0]-vals[1]));
    y1 = y;
    x2 = x+sx;
    y2 = y+sy*fabs(vals[1]/(vals[1]-vals[2]));
    x3 = x+sx*fabs(vals[3]/(vals[3]-vals[2]));
    y3 = y+sy;
    x4 = x;
    y4 = y+sy*fabs(vals[0]/(vals[0]-vals[3]));

    if (u) {
        triangles.push_back(vec2(x1, y1));
        triangles.push_back(vec2(x2, y2));
        triangles.push_back(vec2(x3, y3));

        triangles.push_back(vec2(x1, y1));
        triangles.push_back(vec2(x3, y3));
        triangles.push_back(vec2(x4, y4));
    }

    if (type==5) {
        triangles.push_back(vec2(x, y));
        triangles.push_back(vec2(x1, y1));
        triangles.push_back(vec2(x4, y4));

        triangles.push_back(vec2(x+sx, y+sy));
        triangles.push_back(vec2(x3, y3));
        triangles.push_back(vec2(x2, y2));

    } else if (type==10) {
        triangles.push_back(vec2(x, y+sy));
        triangles.push_back(vec2(x4, y4));
        triangles.push_back(vec2(x3, y3));

        triangles.push_back(vec2(x+sx, y));
        triangles.push_back(vec2(x2, y2));
        triangles.push_back(vec2(x1, y1));
    }
}

// ----------------------------------------------------------------------------

bool ContourLine::init(
        const float *points, int cols, int rows,
        float xmin, float ymin, float xmax, float ymax,
        float threshold) {
    xmin_ = xmin; ymin_ = ymin;
    xmax_ = xmax; ymax_ = ymax;
    threshold_ = threshold;

    float dx, dy;
    dx = (xmax_ - xmin_) / float(rows - 1);
    dy = (ymax_ - ymin_) / float(cols - 1);

    float vals[4];

    lines.clear();

    for (int j=0; j<rows-1; j++) {
        float y = ymin_ + float(j) * dy;
        for (int i=0; i<cols-1; i++) {
            float x = xmin_ + float(i) * dx;

            vals[0] = points[(j  ) * rows + (i  )] - threshold_;
            vals[1] = points[(j  ) * rows + (i+1)] - threshold_;
            vals[2] = points[(j+1) * rows + (i+1)] - threshold_;
            vals[3] = points[(j+1) * rows + (i  )] - threshold_;

            int type = GetCellType(vals);
            switch (type) {
            case 1:
            case 2:
            case 4:
            case 7:
            case 8:
            case 11:
            case 13:
            case 14:
                // One corner
                CreateCorner(lines, type, x, y, dx, dy, vals);
                break;

            case 3:
            case 6:
            case 9:
            case 12:
                // Half
                CreateHalf(lines, type, x, y, dx, dy, vals);
                break;

            case 5:
            case 10:
                // Ambiguity
                float v; bool flag;
                v = (vals[0] + vals[1] + vals[2] + vals[3]) / 4.f;
                flag = v > 0.0f;
                CreateAmbiguity(lines, type, x, y, dx, dy, vals, v, flag);
                break;

            case 0: case 15:
            default:
                // No lines
                break;
            }
        }
    }

    return true;
}

void ContourLine::render() {
    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, lines.data());
    glDrawArrays(GL_LINES, 0, lines.size());

    glDisableClientState(GL_VERTEX_ARRAY);
}

// ----------------------------------------------------------------------------

bool ContourFill::init(
        const float *points, int cols, int rows,
        float xmin, float ymin, float xmax, float ymax,
        float threshold) {
    xmin_ = xmin; ymin_ = ymin;
    xmax_ = xmax; ymax_ = ymax;
    threshold_ = threshold;

    float dx, dy;
    dx = (xmax_ - xmin_) / float(rows - 1);
    dy = (ymax_ - ymin_) / float(cols - 1);

    float vals[4];

    triangles.clear();

    for (int j=0; j<rows-1; j++) {
        float y = ymin_ + float(j) * dy;
        for (int i=0; i<cols-1; i++) {
            float x = xmin_ + float(i) * dx;

            vals[0] = points[(j  ) * rows + (i  )] - threshold_;
            vals[1] = points[(j  ) * rows + (i+1)] - threshold_;
            vals[2] = points[(j+1) * rows + (i+1)] - threshold_;
            vals[3] = points[(j+1) * rows + (i  )] - threshold_;

            int type = GetCellType(vals);
            switch (type) {
            case 1:
            case 2:
            case 4:
            case 7:
            case 8:
            case 11:
            case 13:
            case 14:
                // One corner
                CreateFilledCorner(triangles, type, x, y, dx, dy, vals);
                break;

            case 3:
            case 6:
            case 9:
            case 12:
                // Half
                CreateFilledHalf(triangles, type, x, y, dx, dy, vals);
                break;

            case 5:
            case 10:
                // Ambiguity
                float v; bool flag;
                v = (vals[0] + vals[1] + vals[2] + vals[3]) / 4.f;
                flag = v > 0.0f;
                CreateFilledAmbiguity(triangles, type, x, y, dx, dy, vals, v, flag);
                break;

            case 15:
                // Full filled
                CreateFullFilled(triangles, x, y, dx, dy);
                break;

            case 0:
            default:
                // No lines
                break;
            }
        }
    }

    return true;
}

void ContourFill::render() {
    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, triangles.data());
    glDrawArrays(GL_TRIANGLES, 0, triangles.size());

    glDisableClientState(GL_VERTEX_ARRAY);
}
