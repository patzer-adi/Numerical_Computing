#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "MatrixException.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

class Matrix {
protected:
  double **data;
  int rows;
  int cols;

public:
  // constructors
  Matrix();
  Matrix(int r, int c);
  Matrix(string filename);     // construct from file
  Matrix(const Matrix &other); // copy constructor
  virtual ~Matrix();

  // getters
  int getRows();
  int getCols();
  double getData(int i, int j);
  void setData(int i, int j, double val);

  // I/O methods — the class handles its own input/output
  void readFromConsole();             // ask user for rows, cols, then read
  void readFromFile(string filename); // read from file (auto-detect format)
  void saveToFile(string filename);   // write matrix to file

  // static helper — asks "manual or file?" and returns a Matrix
  static Matrix inputMatrix(string label);

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
  Matrix &operator=(const Matrix &other);
  void copyFrom(Matrix &other);
};

#endif
