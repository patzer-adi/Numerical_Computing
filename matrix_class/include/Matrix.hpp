#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <string>
using namespace std;


class MatrixError {
    string msg;
public:
    MatrixError(string m) {
        this->msg = m;
    }
    string what() { return msg; }
};



class Shape {
public:
    virtual void display() = 0;
    virtual ~Shape() {}
};



class Matrix : public Shape {
    int rows;
    int cols;
    int** data;

public:
    Matrix(int r, int c);

    Matrix(const Matrix& other);

    ~Matrix();

    Matrix& operator=(const Matrix& other);

    Matrix operator+(const Matrix& other);

    Matrix operator-(const Matrix& other);

    int getRows() { return rows; }
    int getCols() { return cols; }
    int** getData() { return data; }

    void display() override;
};



class GaussianElimination {
    int n;
    double** mat;
    double* solution;

public:
    GaussianElimination(int size);
    ~GaussianElimination();

    void takeInput();
    void compute();
    void showSolution();
};


#endif
