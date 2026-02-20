#include "../include/Matrix.hpp"
#include <iostream>
using namespace std;


// Constructor — make a rows x cols grid, fill with 0
Matrix::Matrix(int r, int c) {
    this->rows = r;    // this-> used in constructor
    this->cols = c;

    data = new int*[rows];
    for (int i = 0; i < rows; i++) {
        data[i] = new int[cols];
        for (int j = 0; j < cols; j++) {
            data[i][j] = 0;
        }
    }
}


// Copy constructor — called when we do  Matrix b = a;
// Makes its own fresh memory so changes to b don't affect a
Matrix::Matrix(const Matrix& other) {
    this->rows = other.rows;   // this-> used in constructor
    this->cols = other.cols;

    data = new int*[rows];
    for (int i = 0; i < rows; i++) {
        data[i] = new int[cols];
        for (int j = 0; j < cols; j++) {
            data[i][j] = other.data[i][j];
        }
    }
}


// Destructor — free memory when object goes out of scope
Matrix::~Matrix() {
    for (int i = 0; i < rows; i++) {
        delete[] data[i];
    }
    delete[] data;
}


// Assignment operator — called when we do  a = b;  (both already exist)
Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other) {   // don't copy yourself
        return *this;
    }

    // free old memory first
    for (int i = 0; i < rows; i++) {
        delete[] data[i];
    }
    delete[] data;

    // copy new size and data
    rows = other.rows;
    cols = other.cols;

    data = new int*[rows];
    for (int i = 0; i < rows; i++) {
        data[i] = new int[cols];
        for (int j = 0; j < cols; j++) {
            data[i][j] = other.data[i][j];
        }
    }

    return *this;
}


// display() — overrides the pure virtual from Shape
// Prints the matrix with border pipes
void Matrix::display() {
    for (int i = 0; i < rows; i++) {
        cout << "| ";
        for (int j = 0; j < cols; j++) {
            cout << data[i][j] << "\t";
        }
        cout << "|" << endl;
    }
}