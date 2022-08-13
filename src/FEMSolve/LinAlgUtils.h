#pragma once

template <typename T>
class VectorT : public std::vector<T> {
public:
    VectorT() = default;

    T& operator()(const size_t i) {
        return const_cast<T&>(this->at(i));
    }

    const T& operator()(const size_t i) const {
        return this->at(i);
    }

    VectorT<T>& operator*=(const T x) {
        std::transform(begin(), end(),
            begin(),
            [x](const T a) -> T { return a * x; });

        return *this;
    }

    VectorT<T>& operator/=(const T x) {
        std::transform(begin(), end(),
            begin(),
            [x](const T a) -> T { return a / x; });

        return *this;
    }
};

template <typename T>
class MatrixT : public std::vector<T> {
public:
    MatrixT() = default;

    void resize(size_t rows, size_t cols, T v) {
        rows_ = rows;
        cols_ = cols;
        std::vector<T>::resize(rows * cols, v);
    }

    int rows() const { return rows_; }
    int cols() const { return cols_; }

    T& operator()(const size_t row, const size_t col) {
        return const_cast<T&>(this->at(index(row, col)));
    }

    const T& operator()(const size_t row, const size_t col) const {
        return this->at(index(row, col));
    }

private:
    inline size_t index(const size_t row, const size_t col) const {
        return cols_ * row + col;
    }

private:
    size_t rows_{ 0 }, cols_{ 0 };
};

using Vector = VectorT<float>;
using Matrix = MatrixT<float>;

std::ostream& operator<<(std::ostream& os, const Vector& v);
std::ostream& operator<<(std::ostream& os, const Matrix& m);

void seqv(Matrix& a, Vector& b, Vector& x, int N);
