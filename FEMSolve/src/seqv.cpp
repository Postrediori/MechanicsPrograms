// seqv.cpp

// #include <stdio.h>
#include <math.h>
#include "seqv.h"

void seqv(float **a, float *b, float *x, int N) {
    int i, j, k;

    // 1. Прямой ход
    // Приведение матрицы к треугольному виду

    // Перебираем все строки со 2-й
    for (k=1; k<N; k++) {
        int imax = 0;
        float a11max = fabs(a[k-1][0]);
        // Поиск наибольшего элемента в k-1-й строке
        for (i=1; i<N; i++)
            if (fabs(a[k-1][i])>a11max) {
                a11max = fabs(a[k-1][i]);
                imax = i;
            }

        // Меняем местами imax-й столбец с k-1-м
        float tmp;
        for (i=0; i<N; i++) {
            tmp = a[i][imax];
            a[i][imax] = a[i][k-1];
            a[i][k-1] = tmp;
        }

        // Главный элемент
        float a11 = a[k-1][k-1];

        // Перебираем все строки от текущей
        for (i=k; i<N; i++) {
            float aii = a[i][k-1];
            float ai1 = aii / a11;
            // Изменение компонентов в строке
            for (j=0; j<N; j++)
                a[i][j] -= a[k-1][j] * ai1;

            // Изменение свободных компонентов
            b[i] -= b[k-1] * ai1;
        }
    }

    /*printf("A:\n");
    print_matrix(a, N);
    printf("\n");*/

    // 2. Обратный ход
    // Нахождение корней
    for (i=N-1; i>=0; i--) {
        x[i] = b[i] / a[i][i];
        for (j=i-1; j>=0; j--)
            b[j] -= a[j][i] * x[i];
    }
}
