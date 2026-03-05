#include "../include/Matrix.hpp"
#include <cmath>
#include <iostream>
using namespace std;

// scalar multiplication: every element * scalar
Matrix Matrix::operator*(double scalar) {
  Matrix result(rows, cols);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      result.data[i][j] = data[i][j] * scalar;
  return result;
}

// transpose: swap rows and cols
Matrix Matrix::transpose() {
  Matrix result(cols, rows);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      result.data[j][i] = data[i][j];
  return result;
}

// minor matrix: remove row r and col c, return (n-1)x(n-1)
Matrix Matrix::minorMatrix(int r, int c) {
  if (rows != cols)
    throw MatrixException(
        "minor matrix only works on square matrices... come on");
  if (rows <= 1)
    throw MatrixException(
        "matrix too small to take a minor... it's already 1x1");

  int n = rows;
  Matrix result(n - 1, n - 1);
  int ri = 0;
  for (int i = 0; i < n; i++) {
    if (i == r)
      continue;
    int ci = 0;
    for (int j = 0; j < n; j++) {
      if (j == c)
        continue;
      result.data[ri][ci] = data[i][j];
      ci++;
    }
    ri++;
  }
  return result;
}

// cofactor: (-1)^(r+c) * det(minor(r,c))
double Matrix::cofactor(int r, int c) {
  Matrix minor = minorMatrix(r, c);
  double det = minor.determinant();
  double sign = ((r + c) % 2 == 0) ? 1.0 : -1.0;
  return sign * det;
}

// adjoint: transpose of the cofactor matrix
Matrix Matrix::adjoint() {
  if (rows != cols)
    throw MatrixException(
        "adjoint only works on square matrices... nice try though");

  int n = rows;
  Matrix cofactorMatrix(n, n);

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      cofactorMatrix.data[i][j] = cofactor(i, j);

  return cofactorMatrix.transpose();
}

// inverse: adj(A) / det(A)
Matrix Matrix::inverse() {
  if (rows != cols)
    throw MatrixException(
        "inverse only works on square matrices... everybody knows that");

  double det = determinant();
  if (fabs(det) < 1e-12)
    throw MatrixException(
        "matrix is singular (det = 0)... inverse doesn't exist, just like my "
        "social life 💀");

  Matrix adj = adjoint();
  double invDet = 1.0 / det;

  return adj * invDet;
}
