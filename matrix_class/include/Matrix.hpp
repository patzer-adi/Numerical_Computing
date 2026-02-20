#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <string>
using namespace std;


// -----------------------------------------------
// Simple exception class — used for error messages
// -----------------------------------------------
class MatrixError {
    string msg;
public:
    MatrixError(string m) {
        this->msg = m;   // this-> used here
    }
    string what() { return msg; }
};


// -----------------------------------------------
// Base class — just a simple shape interface
// We use virtual so child classes can override display
// -----------------------------------------------
class Shape {
public:
    virtual void display() = 0;   // pure virtual — forces child to implement
    virtual ~Shape() {}
};


// -----------------------------------------------
// Matrix class — stores a grid of integers
// Inherits from Shape so it must implement display()
// -----------------------------------------------
class Matrix : public Shape {
    int rows;
    int cols;
    int** data;

public:
    // Constructor
    Matrix(int r, int c);

    // Copy constructor — so  Matrix b = a;  works safely
    Matrix(const Matrix& other);

    // Destructor — frees memory
    ~Matrix();

    // Assignment operator — so  a = b;  works safely
    Matrix& operator=(const Matrix& other);

    // Add two matrices
    Matrix operator+(const Matrix& other);

    // Subtract two matrices
    Matrix operator-(const Matrix& other);

    // Getters
    int getRows() { return rows; }
    int getCols() { return cols; }
    int** getData() { return data; }

    // display() overrides the virtual from Shape
    void display() override;
};


// -----------------------------------------------
// GaussianElimination class — solves Ax = b
// -----------------------------------------------
class GaussianElimination {
    int n;
    double** mat;      // augmented matrix [A | b]
    double* solution;  // answers: x1, x2, x3...

public:
    GaussianElimination(int size);
    ~GaussianElimination();

    void takeInput();
    void compute();
    void showSolution();
};


#endif