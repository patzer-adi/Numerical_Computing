#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "MatrixException.hpp"
#include <cmath>
#include <fstream>
#include <iostream>

class Matrix {
protected:
  double **data;
  int rows;
  int cols;

public:
  // Constructors
  Matrix();
  Matrix(int r, int c);
  Matrix(const Matrix &other);
  virtual ~Matrix();

  // Getters
  int getRows();
  int getCols();
  double getData(int i, int j);
  void setData(int i, int j, double val);

  // basic operations
  Matrix add(Matrix other);
  Matrix subtract(Matrix other);
  Matrix multiply(Matrix other);

  // operator overloading
  Matrix operator+(Matrix &other);
  Matrix operator-(Matrix &other);
  Matrix operator*(Matrix &other);

  // determinant
  double determinant();

  // display
  void display();

  // assignment
  void copyFrom(Matrix &other);
};

#endif
