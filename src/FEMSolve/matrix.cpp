// matrix.cpp

#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"

float* create_vector(int N) {
    float *vector;
    vector = (float*) malloc(sizeof(float) * N);
    for (int i=0; i<N; i++)
        vector[i] = 0.0f;
    return vector;
}

float** create_matrix(int N) {
    float **matrix;
    matrix = (float**) malloc(sizeof(float*) * N);
    for (int i=0; i<N; i++) {
        matrix[i] = (float*) malloc(sizeof(float) * N);
        for (int j=0; j<N; j++)
            matrix[i][j] = 0.0f;
    }
    return matrix;
}

void dispose_vector(float* v) {
    free(v);
}

void dispose_matrix(float** m, int N) {
    for (int i=0; i<N; i++)
        free(m[i]);
    free(m);
}

void print_vector(float* v, int N) {
    for (int i=0; i<N; i++)
        printf("%12.4e ", v[i]);
    printf("\n");
}

void print_matrix(float** m, int N) {
    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++)
            printf("%12.4e ", m[i][j]);
        printf("\n");
    }
}
