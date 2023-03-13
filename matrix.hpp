#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>

template<typename Tp = double>
struct Matrix;
template<typename Tp = double>
Matrix<Tp> zeros(size_t rows, size_t cols);
template<typename Tp = double>
Matrix<Tp> ones(size_t rows, size_t cols);


template<typename Tp>
struct Matrix {
    // 简单的定义
    std::unique_ptr<Tp[]> mData{nullptr};
    size_t mRowCount{0};
    size_t mColCount{0};
    //获得具体位置的元素，可以修改
    Tp &operator()(size_t row, size_t col) {
        return mData[row * mColCount + col];
    }
    //查看具体位置的元素
    const Tp &operator()(size_t row, size_t col) const {
        return mData[row * mColCount + col];
    }
    //从外部数据构造，并且抢夺该数据的所有权。外面的数据不需要再delete。
    Matrix(Tp *eData, size_t rows, size_t cols) : mData(eData), mRowCount(rows), mColCount(cols) {}
    //空白构造
    Matrix(size_t rows, size_t cols) : mRowCount(rows), mColCount(cols) {
        mData = std::make_unique<Tp[]>(rows * cols);
    }
    //移动构造，抢夺other的所有权。方便返回值优化。
    Matrix(Matrix &&other) noexcept : mData(std::move(other.mData)), mRowCount(other.mRowCount), mColCount(other.mColCount) {
        other.mRowCount = 0;
        other.mColCount = 0;
    }
    //拷贝构造，深拷贝
    Matrix(const Matrix &other) : mRowCount(other.mRowCount), mColCount(other.mColCount) {
        mData = std::make_unique<Tp[]>(mRowCount * mColCount);
        std::copy(other.mData.get(), other.mData.get() + mRowCount * mColCount, mData.get());
    }
    //移动赋值，抢夺other的所有权。方便返回值优化。
    Matrix &operator=(Matrix &&other) noexcept {
        mData = std::move(other.mData);
        mRowCount = other.mRowCount;
        mColCount = other.mColCount;
        other.mRowCount = 0;
        other.mColCount = 0;
        return *this;
    }
    //拷贝赋值，深拷贝
    Matrix &operator=(const Matrix &other) {
        mRowCount = other.mRowCount;
        mColCount = other.mColCount;
        mData = std::make_unique<Tp[]>(mRowCount * mColCount);
        std::copy(other.mData.get(), other.mData.get() + mRowCount * mColCount, mData.get());
        return *this;
    }
    // 元素广播赋值
    Matrix &operator=(const Tp &value) {
        std::fill(mData.get(), mData.get() + mRowCount * mColCount, value);
        return *this;
    }
    //两个矩阵之间的对应元素操作
    Matrix &operator+=(const Matrix &other) {
        for (size_t i = 0; i < mRowCount; ++i) {
            for (size_t j = 0; j < mColCount; ++j) {
                (*this)(i, j) += other(i, j);
            }
        }
        return *this;
    }
    Matrix &operator*=(const Tp &value) {
        for (size_t i = 0; i < mRowCount; ++i) {
            for (size_t j = 0; j < mColCount; ++j) {
                (*this)(i, j) *= value;
            }
        }
        return *this;
    }
    //类似numpy的友元函数
    // template<typename Tp2>
    // friend Matrix<Tp2> zeros(size_t rows, size_t cols);
    // template<typename Tp2>
    // friend Matrix<Tp2> ones(size_t rows, size_t cols);

    //暴力矩阵乘法, 注意不是inplace操作，而是返回一个新的矩阵。
    Matrix brute_force_matmul(const Matrix &other) const {
        Matrix result(mRowCount, other.mColCount);
        for (size_t i = 0; i < mRowCount; ++i) {
            for (size_t j = 0; j < other.mColCount; ++j) {
                Tp sum = 0;
                for (size_t k = 0; k < mColCount; ++k) {
                    sum += (*this)(i, k) * other(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
    }

    Matrix memory_continue_matmul(const Matrix &other) const {
        auto result = zeros<Tp>(mRowCount, other.mColCount);
        for (size_t i = 0; i < mRowCount; ++i) {
            for (size_t k = 0; k < mColCount; ++k) {
                const auto &temp = (*this)(i, k);
                for (size_t j = 0; j < other.mColCount; ++j) {
                    result(i, j) += temp * other(k, j);
                }
            }
        }
        return result;
    }

    //矩阵差的平方和平均值开根号
    Tp root_mean_square_error(const Matrix &other) const {
        Tp result = 0;
        for (size_t i = 0; i < mRowCount; ++i) {
            for (size_t j = 0; j < mColCount; ++j) {
                auto x = ((*this)(i, j) - other(i, j));
                result += x * x;
            }
        }
        return std::sqrt(result / (mRowCount * mColCount));
    }

    //打印
    friend std::ostream &operator<<(std::ostream &os, const Matrix &mat) {
        os << "Matrix: " << mat.mRowCount << "x" << mat.mColCount << std::endl;
        for (size_t i = 0; i < mat.mRowCount; ++i) {
            os << "\t";
            for (size_t j = 0; j < mat.mColCount; ++j) {
                os << mat(i, j) << " ";
            }
            os << std::endl;
        }
        return os;
    }


    // 用+=和*=实现+和*
    friend Matrix operator+(const Matrix &lhs, const Matrix &rhs) {
        Matrix result(lhs);
        result += rhs;
        return result;
    }
    friend Matrix operator*(const Matrix &lhs, const Tp &rhs) {
        Matrix result(lhs);
        result *= rhs;
        return result;
    }
};
template<typename Tp>
Matrix<Tp> zeros(size_t rows, size_t cols) {
    Tp *data = new Tp[rows * cols]{};
    return Matrix<Tp>(data, rows, cols);
}
template<typename Tp>
Matrix<Tp> ones(size_t rows, size_t cols) {
    Tp *data = new Tp[rows * cols];
    std::fill(data, data + rows * cols, 1);
    return Matrix<Tp>(data, rows, cols);
}