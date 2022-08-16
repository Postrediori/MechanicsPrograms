#include "pch.h"
#include "LinAlgUtils.h"
#include "FEM.h"

constexpr int BufferLen = 256;

void ReadLine(FILE* f, char* string) {
    do {
        fgets(string, BufferLen, f);
    } while (!feof(f) && (string[0] == '/' || string[0] == '\n'));
}

FinitModel::~FinitModel() {
    clear();
}

void FinitModel::clear() {
    component_count = 0;
    nodes.clear();
    fixes.clear();
    loads.clear();
    elems.clear();

    a.clear();
    b.clear();
    u.clear();
    a.clear();
}

void FinitModel::load_from_file(const std::string& fileName) {
    char line[BufferLen] = { 0 };

    /*
     * Load model information from file
     */
    FILE *in;
    in = fopen(fileName.c_str(), "r");

    while (!feof(in)) {
        ReadLine(in, line);

        if (strncmp(line, "NODES ", 6)==0) {
            // Load nodes
            int node_count{ 0 };
            sscanf(line, "NODES %d\n", &node_count);
            component_count = node_count * ModelDimensions;
            nodes.resize(node_count);
            for (int i=0; i<node_count; i++) {
                Node n;
                ReadLine(in, line);
                sscanf(line, "%d %f %f", &n.node, &n.x, &n.y);
                nodes[i] = n;
            }
        }
        else if (strncmp(line, "FIXED ", 6)==0) {
            // Load node fixtures
            int fixes_count{ 0 };
            sscanf(line, "FIXED %d\n", &fixes_count);
            fixes.resize(fixes_count);
            for (int i=0; i< fixes_count; i++) {
                Fixture f;
                ReadLine(in, line);
                sscanf(line, "%d %d", &f.node, &f.axis);
                fixes[i] = f;
            }
        }
        else if (strncmp(line, "LOADS ", 6)==0) {
            // Load node loads
            int loads_count{ 0 };
            sscanf(line, "LOADS %d\n", &loads_count);
            loads.resize(loads_count);
            for (int i=0; i<loads_count; i++) {
                Load l;
                ReadLine(in, line);
                sscanf(line, "%d %f %f", &l.node, &l.px, &l.py);
                loads[i] = l;
            }
        }
        else if (strncmp(line, "ELEMENTS ", 9)==0) {
            // Load elements
            int elements_count{ 0 };
            sscanf(line, "ELEMENTS %d\n", &elements_count);
            elems.resize(elements_count);
            for (int i=0; i< elements_count; i++) {
                Element e;
                ReadLine(in, line);
                sscanf(line, "%d %d %d", &e.elem, &e.nodes[0], &e.nodes[1]);

                // Calculate element parameters
                auto n1 = nodes[e.nodes[0] - 1];
                auto n2 = nodes[e.nodes[1] - 1];
                float dx = n2.x - n1.x;
                float dy = n2.y - n1.y;
                e.length2 = dx * dx + dy * dy;
                e.length = sqrt(e.length2);
                e.cosa = dx / e.length;
                e.sina = dy / e.length;

                elems[i] = e;
            }
        }
        else if (strncmp(line, "E ", 2)==0) {
            sscanf(line, "E %lf\n", &E);
        }
        else if (strncmp(line, "F ", 2)==0) {
            sscanf(line, "F %lf\n", &F);
        }
        else if (strncmp(line, "TITLE ", 6)==0) {
            char s[BufferLen] = { 0 };
            sscanf(line, "TITLE %s\n", s);
            title = s;
        }
    }

    fclose(in);
}

void FinitModel::prepare_solve() {
    // Create vectors
    a.resize(component_count, component_count, 0);
    b.resize(component_count, 0);
    u.resize(component_count, 0);
    n.resize(elems.size(), 0);

    // Fill B matrix
    for (const auto& l : loads) {
        b(l.node * 2 - 2) = l.px;
        b(l.node * 2 - 1) = l.py;
    }

    // Fill A matrix
    for (const auto& e : elems) {
        auto n1 = nodes[e.nodes[0] - 1];
        auto n2 = nodes[e.nodes[1] - 1];
        float sina = e.sina;
        float cosa = e.cosa;
        float length = e.length;

        // Fill K matrix for each element
        float K[4][4];
        K[0][0] = cosa * cosa / length;
        K[0][1] = K[1][0] = cosa * sina / length;
        K[1][1] = sina * sina / length;

        for (int j=0; j<2; j++) {
            for (int m=0; m<2; m++) {
                K[j+2][m+2] = K[j][m];
                K[j+2][m] = K[j][m+2] = -K[j][m];
            }
        }

        // Calcluate global indeces of element nodes
        std::array<int, 4> globalIndeces = {
            n1.node * 2 - 1,
            n1.node * 2,
            n2.node * 2 - 1,
            n2.node * 2
        };

        // Copy values from K to A matrix according to global coords
        for (int j=0; j<4; j++) {
            for (int m=0; m<4; m++) {
                auto p = globalIndeces[j] - 1;
                auto q = globalIndeces[m] - 1;
                a(p, q) += K[j][m];
            }
        }
    }

    for (const auto& f : fixes) {
        int node = (f.node - 1) * 2 + 1 - (f.axis % 2);
        for (int j=0; j<component_count; j++) {
            a(node, j) = 0;
            a(j, node) = 0;
        }
        a(node, node) = 1;
        b(node) = 0;
    }
}

void FinitModel::solve() {
    // Solve system of linear equations A*u=b
    seqv(a, b, u, component_count);

    u /= E * F;

    int k = 0;
    for (const auto& e : elems) {
        auto n1 = nodes[e.nodes[0] - 1];
        auto n2 = nodes[e.nodes[1] - 1];
        float nx = (u(2 * n1.node - 2) - u(2 * n2.node - 2) * (n1.x - n2.x));
        float ny = (u(2 * n1.node - 1) - u(2 * n2.node - 1) * (n1.y - n2.y));
        n(k++) = (nx + ny) / e.length2;
    }
}
