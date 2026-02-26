#pragma once

#include <vector>
#include <string>
#include <iostream>
#include "MatrixException.hpp"

class Matrix {
protected:
    int rows;
    int cols;
    std::vector<std::vector<double>> data;

    void checkSameSize(const Matrix& other) const;
    void checkMultiplicationSize(const Matrix& other) const;
    void checkSquare() const;

public:
    Matrix();
    Matrix(int r, int c);
    Matrix(const std::vector<std::vector<double>>& values);
    virtual ~Matrix();

    int getRows() const;
    int getCols() const;

    double& operator()(int i, int j);
    double operator()(int i, int j) const;

    Matrix add(const Matrix& other) const;
    Matrix subtract(const Matrix& other) const;
    Matrix multiply(const Matrix& other) const;

    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;

    double determinantRecursive() const;
    double determinantGaussian() const;

    virtual void print(std::ostream& os = std::cout) const;
};
