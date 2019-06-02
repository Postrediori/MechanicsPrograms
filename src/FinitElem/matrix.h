// matrix.h
// matrices routines
#ifndef MATRIX_H
#define MATRIX_H

float *create_vector(int N);
float **create_matrix(int N);

void dispose_vector(float *v);
void dispose_matrix(float **m, int N);

void print_vector(float *v, int N);
void print_matrix(float **m, int N);

#endif
// MATRIX_H
