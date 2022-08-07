#include "pch.h"
#include "GraphUtils.h"
#include "MathUtils.h"
#include "ContourPlot.h"

using CellType = uint8_t;
using CellValues = std::array<float, 4>;

namespace CellTypes {
    constexpr CellType None = 0x00;

    constexpr CellType NorthWest = 0x01;
    constexpr CellType NorthEast = 0x02;
    constexpr CellType SouthEast = 0x04;
    constexpr CellType SouthWest = 0x08;

    constexpr CellType All = 0x0F;
}

CellType GetCellType(CellValues vals) {
    using namespace CellTypes;

    CellType type = None;
    if (vals[0]>0.0f) type |= NorthWest;
    if (vals[1]>0.0f) type |= NorthEast;
    if (vals[2]>0.0f) type |= SouthEast;
    if (vals[3]>0.0f) type |= SouthWest;
    return type;
}

void CreateCorner(std::vector<vec2>& lines, CellType type,
                float x, float y, float sx, float sy, CellValues vals) {
    using namespace CellTypes;

    vec2 p1, p2;

    switch (type) {
    case NorthWest:
    case All ^ NorthWest:
        p1 = { x, y + sy * fabs(vals[0] / (vals[0] - vals[3])) };
        p2 = { x + sx * fabs(vals[0] / (vals[0] - vals[1])), y };
        break;
    case NorthEast:
    case All ^ NorthEast:
        p1 = { x + sx * fabs(vals[0] / (vals[0] - vals[1])), y };
        p2 = { x + sx, y + sy * fabs(vals[1] / (vals[1] - vals[2])) };
        break;
    case SouthEast:
    case All ^ SouthEast:
        p1 = { x + sx * fabs(vals[3] / (vals[3] - vals[2])), y + sy };
        p2 = { x + sx, y + sy * fabs(vals[1] / (vals[1] - vals[2])) };
        break;
    case SouthWest:
    case All ^ SouthWest:
        p1 = { x, y + sy * fabs(vals[0] / (vals[0] - vals[3])) };
        p2 = { x + sx * fabs(vals[3] / (vals[3] - vals[2])), y + sy };
        break;
    }

    lines.push_back(p1);
    lines.push_back(p2);
}

void CreateHalf(std::vector<vec2>& lines, CellType type,
              float x, float y, float sx, float sy, CellValues vals) {
    using namespace CellTypes;

    vec2 p1, p2;

    switch (type) {
    case NorthWest | NorthEast:
    case All ^ (NorthWest | NorthEast):
        p1 = { x, y + sy * fabs(vals[0] / (vals[0] - vals[3])) };
        p2 = { x + sx,y + sy * fabs(vals[1] / (vals[1] - vals[2])) };
        break;
    case NorthEast | SouthEast:
    case All ^ (NorthEast | SouthEast):
        p1 = { x + sx * fabs(vals[0] / (vals[0] - vals[1])), y };
        p2 = { x + sx * fabs(vals[3] / (vals[3] - vals[2])), y + sy };
        break;
    }

    lines.push_back(p1);
    lines.push_back(p2);
}

void CreateAmbiguity(std::vector<vec2>& lines, CellType flags,
                   float x, float y, float sx, float sy, CellValues vals, float /*v*/,
                   bool u) {
    using namespace CellTypes;

    vec2 p1, p2, p3, p4;

    if ((flags==(NorthWest | SouthEast) && u) ||
            (flags==(NorthEast | SouthWest) && !u)) {
        p1 = { x, y + sy * fabs(vals[0] / (vals[0] - vals[3])) };
        p2 = { x + sx * fabs(vals[3] / (vals[3] - vals[2])), y + sy };

        p3 = { x + sx * fabs(vals[0] / (vals[0] - vals[1])), y };
        p4 = { x + sx, y + sy * fabs(vals[1] / (vals[1] - vals[2])) };
    }

    if ((flags==(NorthWest | SouthEast) && !u) ||
            (flags== (NorthEast | SouthWest) && u)) {
        p1 = { x, y + sy * fabs(vals[0] / (vals[0] - vals[3])) };
        p2 = { x + sx * fabs(vals[0] / (vals[0] - vals[1])), y };

        p3 = { x + sx * fabs(vals[3] / (vals[3] - vals[2])), y + sy };
        p4 = { x + sx, y + sy * fabs(vals[1] / (vals[1] - vals[2])) };
    }

    lines.push_back(p1);
    lines.push_back(p2);
    lines.push_back(p3);
    lines.push_back(p4);
}

void CreateFullFilled(std::vector<vec2>& triangles,
              float x, float y, float sx, float sy) {
    triangles.emplace_back(x, y);
    triangles.emplace_back(x+sx, y+sy);
    triangles.emplace_back(x, y+sy);

    triangles.emplace_back(x, y);
    triangles.emplace_back(x+sx, y);
    triangles.emplace_back(x+sx, y+sy);
}

void CreateFilledCorner(std::vector<vec2>& triangles, CellType type,
                    float x, float y, float sx, float sy, CellValues vals) {
    using namespace CellTypes;

    float x1, y1;
    float x2, y2;
    vec2 p1, p2;

    switch (type) {
    case NorthWest:
    case All^ NorthWest:
        x1 = x+sx*fabs(vals[0]/(vals[0]-vals[1]));
        y1 = y;
        x2 = x;
        y2 = y+sy*fabs(vals[0]/(vals[0]-vals[3]));
        break;

    case NorthEast:
    case All^ NorthEast:
        x1 = x+sx;
        y1 = y+sy*fabs(vals[1]/(vals[1]-vals[2]));
        x2 = x+sx*fabs(vals[0]/(vals[0]-vals[1]));
        y2 = y;
        break;

    case SouthEast:
    case All^ SouthEast:
        x1 = x+sx*fabs(vals[3]/(vals[3]-vals[2]));
        y1 = y+sy;
        x2 = x+sx;
        y2 = y+sy*fabs(vals[1]/(vals[1]-vals[2]));
        break;

    case SouthWest:
    case All^ SouthWest:
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

void CreateFilledHalf(std::vector<vec2>& triangles, CellType type,
                  float x, float y, float sx, float sy, CellValues vals) {
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

void CreateFilledAmbiguity(std::vector<vec2>& triangles, CellType type,
                       float x, float y, float sx, float sy, CellValues vals,
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
        const std::vector<float>& points, int cols, int rows,
        float xmin, float ymin, float xmax, float ymax,
        float threshold) {
    using namespace CellTypes;

    xmin_ = xmin; ymin_ = ymin;
    xmax_ = xmax; ymax_ = ymax;
    threshold_ = threshold;

    float dx = (xmax_ - xmin_) / float(rows - 1);
    float dy = (ymax_ - ymin_) / float(cols - 1);

    CellValues vals;
    float v{ 0.0f };

    lines.clear();

    for (int j=0; j<rows-1; j++) {
        float y = ymin_ + float(j) * dy;
        for (int i=0; i<cols-1; i++) {
            float x = xmin_ + float(i) * dx;

            vals[0] = points[(j  ) * rows + (i  )] - threshold_;
            vals[1] = points[(j  ) * rows + (i+1)] - threshold_;
            vals[2] = points[(j+1) * rows + (i+1)] - threshold_;
            vals[3] = points[(j+1) * rows + (i  )] - threshold_;

            CellType type = GetCellType(vals);
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
                v = (vals[0] + vals[1] + vals[2] + vals[3]) / 4.f;
                CreateAmbiguity(lines, type, x, y, dx, dy, vals,
                    v, v > 0.0f);
                break;

            case None:
            case All:
                // No lines
                break;

            default:
                break;
            }
        }
    }

    return true;
}

void ContourLine::render(CoordinateFunc xFunc, CoordinateFunc yFunc) const {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, lines.data());
    glDrawArrays(GL_LINES, 0, lines.size());

    glDisableClientState(GL_VERTEX_ARRAY);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_line_style(FL_SOLID, 1);
    for (int i = 0; i < lines.size(); i += 2) {
        const auto& p1 = lines[i];
        const auto& p2 = lines[i + 1];
        fl_line(
            xFunc(p1.x), yFunc(p1.y),
            xFunc(p2.x), yFunc(p2.y)
        );
    }
#endif
}

// ----------------------------------------------------------------------------

bool ContourFill::init(
        const std::vector<float>& points, int cols, int rows,
        float xmin, float ymin, float xmax, float ymax,
        float threshold) {
    using namespace CellTypes;

    xmin_ = xmin; ymin_ = ymin;
    xmax_ = xmax; ymax_ = ymax;
    threshold_ = threshold;

    float dx = (xmax_ - xmin_) / float(rows - 1);
    float dy = (ymax_ - ymin_) / float(cols - 1);

    CellValues vals;
    float v{ 0.0f };

    triangles.clear();

    for (int j=0; j<rows-1; j++) {
        float y = ymin_ + float(j) * dy;
        for (int i=0; i<cols-1; i++) {
            float x = xmin_ + float(i) * dx;

            vals[0] = points[(j  ) * rows + (i  )] - threshold_;
            vals[1] = points[(j  ) * rows + (i+1)] - threshold_;
            vals[2] = points[(j+1) * rows + (i+1)] - threshold_;
            vals[3] = points[(j+1) * rows + (i  )] - threshold_;

            CellType type = GetCellType(vals);
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
                v = (vals[0] + vals[1] + vals[2] + vals[3]) / 4.f;
                CreateFilledAmbiguity(triangles, type, x, y, dx, dy, vals,
                    v, v > 0.0f);
                break;

            case All:
                // Full filled
                CreateFullFilled(triangles, x, y, dx, dy);
                break;

            case None:
                // No lines
                break;

            default:
                break;
            }
        }
    }

    return true;
}

void ContourFill::render(CoordinateFunc xFunc, CoordinateFunc yFunc) const {
#if DRAW_METHOD==DRAW_METHOD_OPENGL
    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, triangles.data());
    glDrawArrays(GL_TRIANGLES, 0, triangles.size());

    glDisableClientState(GL_VERTEX_ARRAY);
#elif DRAW_METHOD==DRAW_METHOD_FLTK
    fl_line_style(FL_SOLID, 1);
    for (int i = 0; i < triangles.size(); i += 3) {
        const auto& p1 = triangles[i];
        const auto& p2 = triangles[i + 1];
        const auto& p3 = triangles[i + 2];
        fl_polygon(
            xFunc(p1.x), yFunc(p1.y),
            xFunc(p2.x), yFunc(p2.y),
            xFunc(p3.x), yFunc(p3.y)
        );
    }
#endif
}
