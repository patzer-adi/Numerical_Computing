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
  Matrix(Matrix &&other) noexcept; // move constructor (Rule of 5)
  virtual ~Matrix();

  // getters
  int getRows() const;
  int getCols() const;
  double getData(int i, int j) const;
  void setData(int i, int j, double val);

  // I/O methods — the class handles its own input/output
  void readFromConsole();             // ask user for rows, cols, then read
  void readFromFile(string filename); // read from file (auto-detect format)
  void saveToFile(string filename);   // write matrix to file

  // static helper — asks "manual or file?" and returns a Matrix
  static Matrix inputMatrix(string label);

  // basic operations
  Matrix add(const Matrix &other) const;
  Matrix subtract(const Matrix &other) const;
  Matrix multiply(const Matrix &other) const;

  // operator overloading
  Matrix operator+(const Matrix &other) const;
  Matrix operator-(const Matrix &other) const;
  Matrix operator*(const Matrix &other) const;
  Matrix operator*(double scalar) const;

  // stream operators (friend functions)
  friend ostream &operator<<(ostream &out, const Matrix &m);
  friend istream &operator>>(istream &in, Matrix &m);

  // element access operator: A(i, j)
  double &operator()(int i, int j);
  double operator()(int i, int j) const; // const overload

  // equality operator
  bool operator==(const Matrix &other) const;

  // transpose
  Matrix transpose() const;

  // determinant
  double determinant() const;

  // inverse and related
  Matrix minorMatrix(int r, int c) const;
  double cofactor(int r, int c) const;
  Matrix adjoint() const;
  Matrix inverse() const;

  // matrix check functions
  bool isSquare() const;
  bool isSymmetric() const;
  bool isIdentity() const;
  bool isNull() const;
  bool isDiagonal() const;
  bool isDiagonallyDominant() const;
  Matrix makeDiagonallyDominant();

  // get raw pointer to a row (for CUDA memory transfers)
  double *getRowPointer(int i);

  // display
  void display() const;

  // assignment
  Matrix &operator=(const Matrix &other);
  Matrix &operator=(Matrix &&other) noexcept; // move assignment (Rule of 5)
  void copyFrom(const Matrix &other);
};

#endif
