#include "pch.h"
#include "LinAlgUtils.h"

std::ostream& operator<<(std::ostream& os, const Vector& v) {
    os << std::setprecision(4) << std::scientific << std::setw(12) << std::setfill(' ');;
    for (const auto& x : v) {
        os << x << ",";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    os << std::setprecision(4) << std::scientific << std::setw(12) << std::setfill(' ');
    for (size_t row = 0; row < m.rows(); row++) {
        for (size_t col = 0; col < m.cols(); col++) {
            if (col > 0) {
                os << ",";
            }
            os << m(col, row);
        }
        os << std::endl;
    }
    return os;
}

void seqv(Matrix& a, Vector& b, Vector& x, int N) {
    // 1. Прямой ход
    // Приведение матрицы к треугольному виду

    // Перебираем все строки со 2-й
    for (int k = 1; k < N; k++) {
        int imax = 0;
        float a11max = fabs(a(k - 1, 0));
        // Поиск наибольшего элемента в k-1-й строке
        for (int i = 1; i < N; i++)
            if (fabs(a(k - 1, i)) > a11max) {
                a11max = fabs(a(k - 1, i));
                imax = i;
            }

        // Меняем местами imax-й столбец с k-1-м
        for (int i = 0; i < N; i++) {
            std::swap(a(i, imax), a(i, k - 1));
        }

        // Главный элемент
        float a11 = a(k - 1, k - 1);

        // Перебираем все строки от текущей
        for (int i = k; i < N; i++) {
            float aii = a(i, k - 1);
            float ai1 = aii / a11;
            // Изменение компонентов в строке
            for (int j = 0; j < N; j++) {
                a(i, j) -= a(k - 1, j) * ai1;
            }

            // Изменение свободных компонентов
            b(i) -= b(k - 1) * ai1;
        }
    }

    /*printf("A:\n");
    print_matrix(a, N);
    printf("\n");*/

    // 2. Обратный ход
    // Нахождение корней
    for (int i = N - 1; i >= 0; i--) {
        x(i) = b(i) / a(i, i);
        for (int j = i - 1; j >= 0; j--) {
            b(j) -= a(j, i) * x(i);
        }
    }
}
