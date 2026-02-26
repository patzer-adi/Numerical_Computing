#include "../include/Matrix.hpp"
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
using namespace std;

// default constructor
Matrix::Matrix() {
  rows = 0;
  cols = 0;
  data = nullptr;
}

// parameterized constructor
Matrix::Matrix(int r, int c) {
  rows = r;
  cols = c;
  data = new double *[rows];
  for (int i = 0; i < rows; i++) {
    data[i] = new double[cols];
    for (int j = 0; j < cols; j++) {
      data[i][j] = 0.0;
    }
  }
}

// copy constructor
Matrix::Matrix(const Matrix &other) {
  rows = other.rows;
  cols = other.cols;
  data = new double *[rows];
  for (int i = 0; i < rows; i++) {
    data[i] = new double[cols];
    for (int j = 0; j < cols; j++) {
      data[i][j] = other.data[i][j];
    }
  }
}

// destructor
Matrix::~Matrix() {
  if (data != nullptr) {
    for (int i = 0; i < rows; i++) {
      delete[] data[i];
    }
    delete[] data;
  }
}

// getters
int Matrix::getRows() { return rows; }
int Matrix::getCols() { return cols; }

double Matrix::getData(int i, int j) {
  if (i < 0 || i >= rows || j < 0 || j >= cols)
    throw MatrixException(
        "bruh you went out of bounds... matrix ain't that big 💀");
  return data[i][j];
}

void Matrix::setData(int i, int j, double val) {
  if (i < 0 || i >= rows || j < 0 || j >= cols)
    throw MatrixException(
        "you're trying to set data outside the matrix... nice try 🤡");
  data[i][j] = val;
}

// copy from another matrix
void Matrix::copyFrom(Matrix &other) {
  // clean up old data if any
  if (data != nullptr) {
    for (int i = 0; i < rows; i++)
      delete[] data[i];
    delete[] data;
  }
  rows = other.rows;
  cols = other.cols;
  data = new double *[rows];
  for (int i = 0; i < rows; i++) {
    data[i] = new double[cols];
    for (int j = 0; j < cols; j++)
      data[i][j] = other.data[i][j];
  }
}

// addition
Matrix Matrix::add(Matrix other) {
  if (rows != other.rows || cols != other.cols)
    throw MatrixException(
        "can't add these matrices... they don't even match bro");
  Matrix result(rows, cols);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      result.data[i][j] = data[i][j] + other.data[i][j];
  return result;
}

Matrix Matrix::operator+(Matrix &other) { return add(other); }

// subtraction
Matrix Matrix::subtract(Matrix other) {
  if (rows != other.rows || cols != other.cols)
    throw MatrixException(
        "subtraction needs same size matrices... this ain't it chief");
  Matrix result(rows, cols);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      result.data[i][j] = data[i][j] - other.data[i][j];
  return result;
}

Matrix Matrix::operator-(Matrix &other) { return subtract(other); }

// multiplication
Matrix Matrix::multiply(Matrix other) {
  if (cols != other.rows)
    throw MatrixException("matrix multiplication dimensions don't work... go "
                          "back to math class 📐");
  Matrix result(rows, other.cols);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < other.cols; j++)
      for (int k = 0; k < cols; k++)
        result.data[i][j] += data[i][k] * other.data[k][j];
  return result;
}

Matrix Matrix::operator*(Matrix &other) { return multiply(other); }

// determinant using gaussian elimination style
double Matrix::determinant() {
  if (rows != cols)
    throw MatrixException(
        "determinant only works on square matrices... you knew that right?");

  int n = rows;

  // make a copy so we dont mess up the original
  double **temp = new double *[n];
  for (int i = 0; i < n; i++) {
    temp[i] = new double[n];
    for (int j = 0; j < n; j++)
      temp[i][j] = data[i][j];
  }

  double det = 1.0;
  for (int k = 0; k < n; k++) {
    // partial pivoting
    int maxRow = k;
    double maxVal = fabs(temp[k][k]);
    for (int i = k + 1; i < n; i++) {
      if (fabs(temp[i][k]) > maxVal) {
        maxVal = fabs(temp[i][k]);
        maxRow = i;
      }
    }

    if (maxVal < 1e-12) {
      // basically zero, matrix is singular
      for (int i = 0; i < n; i++)
        delete[] temp[i];
      delete[] temp;
      return 0.0;
    }

    // swap rows if needed
    if (maxRow != k) {
      double *tempRow = temp[k];
      temp[k] = temp[maxRow];
      temp[maxRow] = tempRow;
      det *= -1; // swapping changes sign
    }

    det *= temp[k][k];

    // eliminate below
    for (int i = k + 1; i < n; i++) {
      double factor = temp[i][k] / temp[k][k];
      for (int j = k; j < n; j++)
        temp[i][j] -= factor * temp[k][j];
    }
  }

  // cleanup
  for (int i = 0; i < n; i++)
    delete[] temp[i];
  delete[] temp;

  return det;
}

// display matrix
void Matrix::display() {
  cout << fixed << setprecision(6);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++)
      cout << setw(14) << data[i][j];
    cout << endl;
  }
  cout << endl;
}
