/*
 * FinitElem
 * v1.4
 */

#include <cstdio>
#include <cstring>
#include "matrix.h"
#include "model.h"

int main(int argc, char **argv) {
    char model_file[256] = {0};
    if (argc>1) {
        strcpy(model_file, argv[1]);
    } else {
        printf("Usage: finitelem [model file]\n\n");
        return 0;
    }


    FinitModel model;

    model.load_from_file(model_file);
    printf("Loaded finite model from file %s\n", model_file);
    printf("Nodes: %d\n", model.node_count);
    printf("Fixed: %d\n", model.fixed_count);
    printf("Loads: %d\n", model.load_count);
    printf("Elems: %d\n", model.elem_count);

    /*
     * Solution
     */
    model.prepare_solve();
    // Print matrices A and B
    printf("A and B matrices created\n");
    printf("A:\n");
    print_matrix(model.a, model.component_count);

    printf("B:\n");
    print_vector(model.b, model.component_count);

    // Solve equastion A * U = B
    model.solve();

    printf("U:\n");
    print_vector(model.u, model.component_count);

    printf("N:\n");
    print_vector(model.n, model.elem_count);

    return 0;
}

