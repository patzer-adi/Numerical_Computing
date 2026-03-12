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
  Matrix operator+(const Matrix &other);
  Matrix operator-(const Matrix &other);
  Matrix operator*(const Matrix &other);
  Matrix operator*(double scalar);

  // stream operators (friend functions)
  friend ostream &operator<<(ostream &out, const Matrix &m);
  friend istream &operator>>(istream &in, Matrix &m);

  // element access operator: A(i, j)
  double &operator()(int i, int j);

  // equality operator
  bool operator==(const Matrix &other);

  // transpose
  Matrix transpose();

  // determinant
  double determinant();

  // inverse and related
  Matrix minorMatrix(int r, int c);
  double cofactor(int r, int c);
  Matrix adjoint();
  Matrix inverse();

  // matrix check functions
  bool isSquare();
  bool isSymmetric();
  bool isIdentity();
  bool isNull();
  bool isDiagonal();
  bool isDiagonallyDominant();
  Matrix makeDiagonallyDominant();

  // get raw pointer to a row (for CUDA memory transfers)
  double *getRowPointer(int i);

  // display
  void display();

  // assignment
  Matrix &operator=(const Matrix &other);
  void copyFrom(Matrix &other);
};

#endif
