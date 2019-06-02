// model.cpp

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "utils.h"
#include "matrix.h"
#include "seqv.h"
#include "model.h"

FinitModel::FinitModel() :
        component_count(0), node_count(0), nodes(NULL),
        fixed_count(0), fixed(NULL),
        load_count(0), loads(NULL),
        elem_count(0), elems(NULL),
        E(1.0), F(1.0) {
    strcpy(title, "");
}

FinitModel::~FinitModel() {
    clear();
}

void FinitModel::clear() {
    if (nodes) {
        free(nodes);
        nodes = NULL;
        node_count = 0;
    }

    if (fixed) {
        free(fixed);
        fixed = NULL;
        fixed_count = 0;
    }

    if (loads) {
        free(loads);
        loads = NULL;
        load_count = 0;
    }

    if (elems) {
        free(elems);
        elems = NULL;
        elem_count = 0;
    }

    if (a) {
        dispose_matrix(a, component_count);
        a = NULL;
    }

    if (b) {
        dispose_vector(b);
        b = NULL;
    }

    if (u) {
        dispose_vector(u);
        u = NULL;
    }

    if (n) {
        dispose_vector(n);
        n = NULL;
    }

    component_count = 0;
}

void FinitModel::load_from_file(const char* name) {
    char line[MAXSTRLEN];

    /*
     * Load model information from file
     */
    FILE *in;
    in = fopen(name, "r");

    while (!feof(in)) {
        read_string(in, line);

        if (strncmp(line, "NODES ", 6)==0) {
            // Load nodes
            sscanf(line, "NODES %d\n", &node_count);
            component_count = node_count * MODEL_DIMENSIONS;
            nodes = (NODE *) malloc(sizeof(NODE)*node_count);
            for (int i=0; i<node_count; i++) {
                NODE n;
                read_string(in, line);
                sscanf(line, "%d %f %f", &n.node, &n.x, &n.y);
                nodes[i] = n;
            }

        } else if (strncmp(line, "FIXED ", 6)==0) {
            // Load node fixtures
            sscanf(line, "FIXED %d\n", &fixed_count);
            fixed = (FIX *) malloc(sizeof(FIX)*fixed_count);
            for (int i=0; i<fixed_count; i++) {
                FIX f;
                read_string(in, line);
                sscanf(line, "%d %d", &f.node, &f.axis);
                fixed[i] = f;
            }

        } else if (strncmp(line, "LOADS ", 6)==0) {
            // Load node loads
            sscanf(line, "LOADS %d\n", &load_count);
            loads = (LOAD *) malloc(sizeof(LOAD)*load_count);
            for (int i=0; i<load_count; i++) {
                LOAD l;
                read_string(in, line);
                sscanf(line, "%d %f %f", &l.node, &l.px, &l.py);
                loads[i] = l;
            }

        } else if (strncmp(line, "ELEMENTS ", 9)==0) {
            // Load elements
            sscanf(line, "ELEMENTS %d\n", &elem_count);
            elems = (ELEM *) malloc(sizeof(ELEM)*elem_count);
            for (int i=0; i<elem_count; i++) {
                ELEM e;
                read_string(in, line);
                sscanf(line, "%d %d %d", &e.elem, &e.nodes[0], &e.nodes[1]);

                // Calculate element parameters
                NODE n1 = nodes[e.nodes[0]-1], n2 = nodes[e.nodes[1]-1];
                float dx = n2.x - n1.x;
                float dy = n2.y - n1.y;
                e.length = sqrt(dx*dx+dy*dy);
                // float alpha = atan2(dy, dx);
                e.cosa = dx / e.length;// cos(alpha);
                e.sina = dy / e.length;// sin(alpha);

                elems[i] = e;
            }

        } else if (strncmp(line, "E ", 2)==0) {
            sscanf(line, "E %f\n", &E);

        } else if (strncmp(line, "F ", 2)==0) {
            sscanf(line, "F %f\n", &F);

        } else if (strncmp(line, "TITLE ", 6)==0) {
            strncpy(title, line + 6, strlen(line) - 7);
            // sscanf(line, "TITLE %s\n", (char *)title);

        }
    }

    fclose(in);
}

void FinitModel::prepare_solve() {
    // Create vectors
    b = create_vector(component_count);
    a = create_matrix(component_count);
    u = create_vector(component_count);
    n = create_vector(elem_count);

    // Fill B matrix
    for (int i=0; i<load_count; i++) {
        LOAD l = loads[i];
        b[l.node*2-2] = l.px;
        b[l.node*2-1] = l.py;
    }

    // Fill A matrix
    for (int i=0; i<elem_count; i++) {
        ELEM e = elems[i];
        NODE n1 = nodes[e.nodes[0]-1], n2 = nodes[e.nodes[1]-1];
        float sina = e.sina, cosa = e.cosa, length = e.length;

        // Calcluate global indeces of element nodes
        int GlobalIndeces[4];
        GlobalIndeces[0] = n1.node*2-1;
        GlobalIndeces[1] = n1.node*2;
        GlobalIndeces[2] = n2.node*2-1;
        GlobalIndeces[3] = n2.node*2;

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

        // Copy values from K to A matrix according to global coords
        for (int j=0; j<4; j++) {
            for (int m=0; m<4; m++) {
                a[GlobalIndeces[j]-1][GlobalIndeces[m]-1] += K[j][m];
            }
        }
    }

    for (int i=0; i<fixed_count; i++) {
        FIX f = fixed[i];
        int node = (f.node - 1) * 2 + 1 - (f.axis % 2);
        for (int j=0; j<component_count; j++) {
            a[node][j] = 0.0;
            a[j][node] = 0.0;
        }
        a[node][node] = 1.0;
        b[node] = 0.0;
    }
}

void FinitModel::solve() {
    seqv(a, b, u, component_count);

    for (int i=0; i<component_count; i++)
        u[i] = u[i] / (E * F);

    for (int i=0; i<elem_count; i++) {
        ELEM e = elems[i];
        NODE n1 = nodes[e.nodes[0]-1], n2 = nodes[e.nodes[1]-1];
        n[i] = ((u[2*n1.node - 2] - u[2*n2.node - 2])*(n1.x - n2.x) +
                (u[2*n1.node - 1] - u[2*n2.node - 1])*(n1.y - n2.y)) / (e.length * e.length);
    }
}
