#include "pch.h"
#include "GraphUtils.h"
#include "MathUtils.h"
#include "ContourPlot.h"

using CellType = uint8_t;
using CellValues = std::array<float, 4>;

namespace CellTypes {
    constexpr CellType Nothing = 0x00;

    constexpr CellType NorthWest = 0x01;
    constexpr CellType NorthEast = 0x02;
    constexpr CellType SouthEast = 0x04;
    constexpr CellType SouthWest = 0x08;

    constexpr CellType All = 0x0F;
}

CellType GetCellType(CellValues vals) {
    using namespace CellTypes;

    CellType type = Nothing;
    type |= (vals[0] > 0) ? NorthWest : 0;
    type |= (vals[1] > 0) ? NorthEast : 0;
    type |= (vals[2] > 0) ? SouthEast : 0;
    type |= (vals[3] > 0) ? SouthWest : 0;
    return type;
}

void CreateCorner(std::vector<HMM_Vec2>& lines, CellType type,
                HMM_Vec2 c0, HMM_Vec2 s0, CellValues vals) {
    using namespace CellTypes;

    HMM_Vec2 p1, p2;

    switch (type) {
    case NorthWest:
    case All ^ NorthWest:
        p1 = {{ 0, fabs(vals[0] / (vals[0] - vals[3])) }};
        p2 = {{ fabs(vals[0] / (vals[0] - vals[1])), 0 }};
        break;
    case NorthEast:
    case All ^ NorthEast:
        p1 = {{ fabs(vals[0] / (vals[0] - vals[1])), 0 }};
        p2 = {{ 1, fabs(vals[1] / (vals[1] - vals[2])) }};
        break;
    case SouthEast:
    case All ^ SouthEast:
        p1 = {{ fabs(vals[3] / (vals[3] - vals[2])), 1 }};
        p2 = {{ 1, fabs(vals[1] / (vals[1] - vals[2])) }};
        break;
    case SouthWest:
    case All ^ SouthWest:
        p1 = {{ 0, fabs(vals[0] / (vals[0] - vals[3])) }};
        p2 = {{ fabs(vals[3] / (vals[3] - vals[2])), 1 }};
        break;
    }

    lines.push_back(c0 + s0 * p1);
    lines.push_back(c0 + s0 * p2);
}

void CreateHalf(std::vector<HMM_Vec2>& lines, CellType type,
              HMM_Vec2 c0, HMM_Vec2 s0, CellValues vals) {
    using namespace CellTypes;

    HMM_Vec2 p1, p2;

    switch (type) {
    case NorthWest | NorthEast:
    case All ^ (NorthWest | NorthEast):
        p1 = {{ 0, fabs(vals[0] / (vals[0] - vals[3])) }};
        p2 = {{ 1, fabs(vals[1] / (vals[1] - vals[2])) }};
        break;
    case NorthEast | SouthEast:
    case All ^ (NorthEast | SouthEast):
        p1 = {{ fabs(vals[0] / (vals[0] - vals[1])), 0 }};
        p2 = {{ fabs(vals[3] / (vals[3] - vals[2])), 1 }};
        break;
    }

    lines.push_back(c0 + s0 * p1);
    lines.push_back(c0 + s0 * p2);
}

void CreateAmbiguity(std::vector<HMM_Vec2>& lines, CellType flags,
                   HMM_Vec2 c0, HMM_Vec2 s0, CellValues vals, bool u) {
    using namespace CellTypes;

    HMM_Vec2 p1, p2, p3, p4;

    if ((flags ==(NorthWest | SouthEast) && u) ||
            (flags ==(NorthEast | SouthWest) && !u)) {
        p1 = {{ 0, fabs(vals[0] / (vals[0] - vals[3])) }};
        p2 = {{ fabs(vals[3] / (vals[3] - vals[2])), 1 }};

        p3 = {{ fabs(vals[0] / (vals[0] - vals[1])), 0 }};
        p4 = {{ 1, fabs(vals[1] / (vals[1] - vals[2])) }};
    }

    if ((flags ==(NorthWest | SouthEast) && !u) ||
            (flags == (NorthEast | SouthWest) && u)) {
        p1 = {{ 0, fabs(vals[0] / (vals[0] - vals[3])) }};
        p2 = {{ fabs(vals[0] / (vals[0] - vals[1])), 0 }};

        p3 = {{ fabs(vals[3] / (vals[3] - vals[2])), 1 }};
        p4 = {{ 1, fabs(vals[1] / (vals[1] - vals[2])) }};
    }

    lines.push_back(c0 + s0 * p1);
    lines.push_back(c0 + s0 * p2);
    lines.push_back(c0 + s0 * p3);
    lines.push_back(c0 + s0 * p4);
}

void CreateFullFilled(std::vector<HMM_Vec2>& triangles,
              HMM_Vec2 c0, HMM_Vec2 s0) {
    HMM_Vec2 c1 = c0 + s0 * HMM_Vec2{ 0, 0 };
    HMM_Vec2 c2 = c0 + s0 * HMM_Vec2{ 1, 0 };
    HMM_Vec2 c3 = c0 + s0 * HMM_Vec2{ 1, 1 };
    HMM_Vec2 c4 = c0 + s0 * HMM_Vec2{ 0, 1 };

    triangles.push_back(c1);
    triangles.push_back(c3);
    triangles.push_back(c4);

    triangles.push_back(c1);
    triangles.push_back(c2);
    triangles.push_back(c3);
}

void CreateFilledCorner(std::vector<HMM_Vec2>& triangles, CellType type,
                    HMM_Vec2 c0, HMM_Vec2 s0, CellValues vals) {
    using namespace CellTypes;

    HMM_Vec2 c1 = c0 + s0 * HMM_Vec2{ 0, 0 };
    HMM_Vec2 c2 = c0 + s0 * HMM_Vec2{ 1, 0 };
    HMM_Vec2 c3 = c0 + s0 * HMM_Vec2{ 1, 1 };
    HMM_Vec2 c4 = c0 + s0 * HMM_Vec2{ 0, 1 };

    HMM_Vec2 p1, p2;

    switch (type) {
    case NorthWest:
    case All ^ NorthWest:
        p1 = {{fabs(vals[0]/(vals[0]-vals[1])), 0}};
        p2 = {{0, fabs(vals[0]/(vals[0]-vals[3]))}};
        break;

    case NorthEast:
    case All ^ NorthEast:
        p1 = {{1, fabs(vals[1]/(vals[1]-vals[2]))}};
        p2 = {{fabs(vals[0]/(vals[0]-vals[1])), 0}};
        break;

    case SouthEast:
    case All ^ SouthEast:
        p1 = {{fabs(vals[3]/(vals[3]-vals[2])), 1}};
        p2 = {{1, fabs(vals[1]/(vals[1]-vals[2]))}};
        break;

    case SouthWest:
    case All ^ SouthWest:
        p1 = {{0, fabs(vals[0]/(vals[0]-vals[3]))}};
        p2 = {{fabs(vals[3]/(vals[3]-vals[2])), 1}};
        break;
    }

    HMM_Vec2 d1 = c0 + s0 * p1;
    HMM_Vec2 d2 = c0 + s0 * p2;

    switch (type) {
    case NorthWest:
        triangles.push_back(c1);
        triangles.push_back(d1);
        triangles.push_back(d2);
        break;

    case (All ^ NorthWest):
        triangles.push_back(d1);
        triangles.push_back(c2);
        triangles.push_back(c3);

        triangles.push_back(d1);
        triangles.push_back(c3);
        triangles.push_back(d2);

        triangles.push_back(d2);
        triangles.push_back(c3);
        triangles.push_back(c4);
        break;
    
    case NorthEast:
        triangles.push_back(c2);
        triangles.push_back(d1);
        triangles.push_back(d2);
        break;
    
    case (All ^ NorthEast):
        triangles.push_back(c1);
        triangles.push_back(d2);
        triangles.push_back(c4);

        triangles.push_back(c4);
        triangles.push_back(d2);
        triangles.push_back(d1);

        triangles.push_back(c4);
        triangles.push_back(d1);
        triangles.push_back(c3);
        break;

    case SouthEast:
        triangles.push_back(c3);
        triangles.push_back(d1);
        triangles.push_back(d2);
        break;
    
    case (All ^ SouthEast):
        triangles.push_back(c1);
        triangles.push_back(c2);
        triangles.push_back(d2);

        triangles.push_back(c1);
        triangles.push_back(d2);
        triangles.push_back(d1);

        triangles.push_back(c1);
        triangles.push_back(d1);
        triangles.push_back(c4);
        break;
    
    case SouthWest:
        triangles.push_back(c4);
        triangles.push_back(d1);
        triangles.push_back(d2);
        break;
    
    case (All ^ SouthWest):
        triangles.push_back(c1);
        triangles.push_back(c2);
        triangles.push_back(d1);

        triangles.push_back(d1);
        triangles.push_back(c2);
        triangles.push_back(d2);

        triangles.push_back(c2);
        triangles.push_back(c3);
        triangles.push_back(d2);
        break;

    default:
        break;
    }
}

void CreateFilledHalf(std::vector<HMM_Vec2>& triangles, CellType type,
                  HMM_Vec2 c0, HMM_Vec2 s0, CellValues vals) {
    using namespace CellTypes;

    HMM_Vec2 p1, p2, p3, p4;

    switch (type) {
    case (NorthWest | NorthEast):
        p1 = {{0, 0}};
        p2 = {{1, 0}};
        p3 = {{1, fabs(vals[1]/(vals[1]-vals[2]))}};
        p4 = {{0, fabs(vals[0]/(vals[0]-vals[3]))}};
        break;

    case (SouthEast | SouthWest):
        p1 = {{0, fabs(vals[0]/(vals[0]-vals[3]))}};
        p2 = {{1, fabs(vals[1]/(vals[1]-vals[2]))}};
        p3 = {{1, 1}};
        p4 = {{0, 1}};
        break;

    case (NorthEast | SouthEast):
        p1 = {{fabs(vals[0]/(vals[0]-vals[1])), 0}};
        p2 = {{1, 0}};
        p3 = {{1, 1}};
        p4 = {{fabs(vals[3]/(vals[3]-vals[2])), 1}};
        break;

    case (NorthWest | SouthWest):
        p1 = {{0, 0}};
        p2 = {{fabs(vals[0]/(vals[0]-vals[1])), 0}};
        p3 = {{fabs(vals[3]/(vals[3]-vals[2])), 1}};
        p4 = {{0, 1}};
        break;
    }

    HMM_Vec2 d1 = c0 + s0 * p1;
    HMM_Vec2 d2 = c0 + s0 * p2;
    HMM_Vec2 d3 = c0 + s0 * p3;
    HMM_Vec2 d4 = c0 + s0 * p4;

    triangles.push_back(d1);
    triangles.push_back(d3);
    triangles.push_back(d4);

    triangles.push_back(d1);
    triangles.push_back(d2);
    triangles.push_back(d3);
}

void CreateFilledAmbiguity(std::vector<HMM_Vec2>& triangles, CellType type,
                       HMM_Vec2 c0, HMM_Vec2 s0, CellValues vals, bool u) {
    using namespace CellTypes;

    HMM_Vec2 c1 = c0 + s0 * HMM_Vec2{ 0, 0 };
    HMM_Vec2 c2 = c0 + s0 * HMM_Vec2{ 1, 0 };
    HMM_Vec2 c3 = c0 + s0 * HMM_Vec2{ 1, 1 };
    HMM_Vec2 c4 = c0 + s0 * HMM_Vec2{ 0, 1 };

    HMM_Vec2 p1 = c0 + s0 * HMM_Vec2{ fabs(vals[0] / (vals[0] - vals[1])), 0 };
    HMM_Vec2 p2 = c0 + s0 * HMM_Vec2{ 1, fabs(vals[1] / (vals[1] - vals[2])) };
    HMM_Vec2 p3 = c0 + s0 * HMM_Vec2{ fabs(vals[3] / (vals[3] - vals[2])), 1 };
    HMM_Vec2 p4 = c0 + s0 * HMM_Vec2{ 0, fabs(vals[0] / (vals[0] - vals[3])) };

    if (u) {
        triangles.push_back(p1);
        triangles.push_back(p2);
        triangles.push_back(p3);

        triangles.push_back(p1);
        triangles.push_back(p3);
        triangles.push_back(p4);
    }

    if (type == (NorthWest | SouthEast)) {
        triangles.push_back(c1);
        triangles.push_back(p1);
        triangles.push_back(p4);

        triangles.push_back(c3);
        triangles.push_back(p3);
        triangles.push_back(p2);
    }
    else if (type == (All ^ (NorthWest | SouthEast))) {
        triangles.push_back(c4);
        triangles.push_back(p4);
        triangles.push_back(p3);

        triangles.push_back(c2);
        triangles.push_back(p2);
        triangles.push_back(p1);
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

    HMM_Vec2 s0 = (HMM_Vec2{ xmax_, ymax_ } - HMM_Vec2{ xmin_, ymin_ }) /
        HMM_Vec2{ static_cast<float>(cols - 1), static_cast<float>(rows - 1) };

    CellValues vals;
    float v{ 0.0f };

    lines.clear();

    for (int j=0; j<rows-1; j++) {
        for (int i=0; i<cols-1; i++) {
            HMM_Vec2 c0 = HMM_Vec2{ xmin_, ymin_ } + HMM_Vec2{ static_cast<float>(i), static_cast<float>(j) } *s0;

            vals[0] = points[(j  ) * rows + (i  )] - threshold_;
            vals[1] = points[(j  ) * rows + (i+1)] - threshold_;
            vals[2] = points[(j+1) * rows + (i+1)] - threshold_;
            vals[3] = points[(j+1) * rows + (i  )] - threshold_;

            CellType type = GetCellType(vals);
            switch (type) {
            case NorthWest:
            case (All ^ NorthWest):
            case NorthEast:
            case (All ^ NorthEast):
            case SouthEast:
            case (All ^ SouthEast):
            case SouthWest:
            case (All ^ SouthWest):
                // One corner
                CreateCorner(lines, type, c0, s0, vals);
                break;

            case (NorthWest | NorthEast):
            case (NorthEast | SouthEast):
            case (SouthEast | SouthWest):
            case (SouthWest | NorthWest):
                // Half
                CreateHalf(lines, type, c0, s0, vals);
                break;

            case NorthWest | SouthEast:
            case All ^ (NorthWest | SouthEast):
                // Ambiguity
                v = (vals[0] + vals[1] + vals[2] + vals[3]) / 4.f;
                CreateAmbiguity(lines, type, c0, s0, vals, v > 0.0f);
                break;

            case Nothing:
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
    for (size_t i = 0; i < lines.size(); i += 2) {
        const auto& p1 = lines[i];
        const auto& p2 = lines[i + 1];
        fl_line(
            xFunc(p1.X), yFunc(p1.Y),
            xFunc(p2.X), yFunc(p2.Y)
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

    HMM_Vec2 s0 = (HMM_Vec2{ xmax_, ymax_ } - HMM_Vec2{ xmin_, ymin_ }) /
        HMM_Vec2{ static_cast<float>(cols - 1), static_cast<float>(rows - 1) };

    CellValues vals;
    float v{ 0.0f };

    triangles.clear();

    for (int j=0; j<rows-1; j++) {
        for (int i=0; i<cols-1; i++) {
            HMM_Vec2 c0 = HMM_Vec2{ xmin_, ymin_ } + HMM_Vec2{ static_cast<float>(i), static_cast<float>(j) } *s0;

            vals[0] = points[(j  ) * rows + (i  )] - threshold_;
            vals[1] = points[(j  ) * rows + (i+1)] - threshold_;
            vals[2] = points[(j+1) * rows + (i+1)] - threshold_;
            vals[3] = points[(j+1) * rows + (i  )] - threshold_;

            CellType type = GetCellType(vals);
            switch (type) {
            case NorthWest:
            case (All ^ NorthWest):
            case NorthEast:
            case (All ^ NorthEast):
            case SouthEast:
            case (All ^ SouthEast):
            case SouthWest:
            case (All ^ SouthWest):
                // One corner
                CreateFilledCorner(triangles, type, c0, s0, vals);
                break;

            case (NorthWest | NorthEast):
            case (NorthEast | SouthEast):
            case (SouthEast | SouthWest):
            case (SouthWest | NorthWest):
                // Half
                CreateFilledHalf(triangles, type, c0, s0, vals);
                break;

            case (NorthWest | SouthEast):
            case (All ^ (NorthWest | SouthEast)):
                // Ambiguity
                v = (vals[0] + vals[1] + vals[2] + vals[3]) / 4.f;
                CreateFilledAmbiguity(triangles, type, c0, s0, vals, v > 0.0f);
                break;

            case All:
                // Full filled
                CreateFullFilled(triangles, c0, s0);
                break;

            case Nothing:
                // Don't fill
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
    for (size_t i = 0; i < triangles.size(); i += 3) {
        const auto& p1 = triangles[i];
        const auto& p2 = triangles[i + 1];
        const auto& p3 = triangles[i + 2];
        fl_polygon(
            xFunc(p1.X), yFunc(p1.Y),
            xFunc(p2.X), yFunc(p2.Y),
            xFunc(p3.X), yFunc(p3.Y)
        );
    }
#endif
}
