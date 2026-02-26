#include "../include/GaussianElimination.hpp"
#include <cmath>
#include <iostream>
using namespace std;

GaussianElimination::GaussianElimination() : Matrix() {}
GaussianElimination::GaussianElimination(int r, int c) : Matrix(r, c) {}

// gaussian elimination WITH partial pivoting
double *GaussianElimination::solveWithPivoting(double *b, int n) {
  if (rows != cols || rows != n)
    throw MatrixException(
        "matrix dimensions are sus for gaussian elimination ");

  // make copies so we dont destroy original
  double **A = new double *[n];
  for (int i = 0; i < n; i++) {
    A[i] = new double[n];
    for (int j = 0; j < n; j++)
      A[i][j] = data[i][j];
  }

  double *rhs = new double[n];
  for (int i = 0; i < n; i++)
    rhs[i] = b[i];

  // forward elimination with pivoting
  for (int k = 0; k < n; k++) {
    // find pivot (max in column)
    int maxRow = k;
    double maxVal = fabs(A[k][k]);
    for (int i = k + 1; i < n; i++) {
      if (fabs(A[i][k]) > maxVal) {
        maxVal = fabs(A[i][k]);
        maxRow = i;
      }
    }

    if (maxVal < 1e-12)
      throw MatrixException("matrix is singular... it has no solution, just "
                            "like my love life");

    // swap rows
    if (maxRow != k) {
      double *tempRow = A[k];
      A[k] = A[maxRow];
      A[maxRow] = tempRow;

      double tempVal = rhs[k];
      rhs[k] = rhs[maxRow];
      rhs[maxRow] = tempVal;
    }

    // eliminate below
    for (int i = k + 1; i < n; i++) {
      double factor = A[i][k] / A[k][k];
      for (int j = k; j < n; j++)
        A[i][j] -= factor * A[k][j];
      rhs[i] -= factor * rhs[k];
    }
  }

  // back substitution
  double *x = new double[n];
  for (int i = n - 1; i >= 0; i--) {
    double sum = rhs[i];
    for (int j = i + 1; j < n; j++)
      sum -= A[i][j] * x[j];
    if (fabs(A[i][i]) < 1e-12)
      throw MatrixException(
          "zero on diagonal during back sub... we're cooked 🍳");
    x[i] = sum / A[i][i];
  }

  // cleanup
  for (int i = 0; i < n; i++)
    delete[] A[i];
  delete[] A;
  delete[] rhs;

  return x;
}

// gaussian elimination WITHOUT pivoting
double *GaussianElimination::solveWithoutPivoting(double *b, int n) {
  if (rows != cols || rows != n)
    throw MatrixException(
        "matrix dimensions are sus for gaussian elimination ");

  // make copies
  double **A = new double *[n];
  for (int i = 0; i < n; i++) {
    A[i] = new double[n];
    for (int j = 0; j < n; j++)
      A[i][j] = data[i][j];
  }

  double *rhs = new double[n];
  for (int i = 0; i < n; i++)
    rhs[i] = b[i];

  // forward elimination without pivoting
  for (int k = 0; k < n; k++) {
    if (fabs(A[k][k]) < 1e-12)
      throw MatrixException(
          "zero pivot found without pivoting... shoulda used pivoting bro");

    for (int i = k + 1; i < n; i++) {
      double factor = A[i][k] / A[k][k];
      for (int j = k; j < n; j++)
        A[i][j] -= factor * A[k][j];
      rhs[i] -= factor * rhs[k];
    }
  }

  // back substitution
  double *x = new double[n];
  for (int i = n - 1; i >= 0; i--) {
    double sum = rhs[i];
    for (int j = i + 1; j < n; j++)
      sum -= A[i][j] * x[j];
    if (fabs(A[i][i]) < 1e-12)
      throw MatrixException("zero diagonal in back sub without pivoting");
    x[i] = sum / A[i][i];
  }

  // cleanup
  for (int i = 0; i < n; i++)
    delete[] A[i];
  delete[] A;
  delete[] rhs;

  return x;
}
