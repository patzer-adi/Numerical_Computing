#include "../include/LUDecomposition.hpp"
#include <cmath>
#include <iostream>
using namespace std;

// Cholesky constructors
Cholesky::Cholesky() : LUDecomposition() {}
Cholesky::Cholesky(int r, int c) : LUDecomposition(r, c) {}

// Cholesky decomposition
// for symmetric positive definite matrices ONLY
// Decompose A into L*L^T, then solve Ly = b, then L^T x = y
double *Cholesky::solve(double *b, int n) {
  if (rows != cols || rows != n)
    throw MatrixException("matrix dimensions don't match for Cholesky");

  // step 1: check if matrix is symmetric
  if (!isSymmetric())
    throw MatrixException(
        "matrix is not symmetric... Cholesky only works on symmetric positive "
        "definite matrices. try Doolittle or Crout instead 🤷");

  // allocate L
  double **L = new double *[n];
  for (int i = 0; i < n; i++) {
    L[i] = new double[n];
    for (int j = 0; j < n; j++)
      L[i][j] = 0.0;
  }

  // decompose A = L * L^T
  for (int i = 0; i < n; i++) {
    for (int j = 0; j <= i; j++) {
      double sum = 0.0;

      if (i == j) {
        // diagonal element
        for (int k = 0; k < j; k++)
          sum += L[j][k] * L[j][k];
        double val = data[j][j] - sum;
        if (val <= 0.0) {
          // cleanup before throwing
          for (int x = 0; x < n; x++)
            delete[] L[x];
          delete[] L;
          throw MatrixException(
              "matrix is not positive definite... Cholesky can't decompose "
              "this. the diagonal went negative 😬");
        }
        L[j][j] = sqrt(val);
      } else {
        // off-diagonal element
        for (int k = 0; k < j; k++)
          sum += L[i][k] * L[j][k];
        if (fabs(L[j][j]) < 1e-12) {
          for (int x = 0; x < n; x++)
            delete[] L[x];
          delete[] L;
          throw MatrixException("zero on L diagonal in Cholesky... rip");
        }
        L[i][j] = (data[i][j] - sum) / L[j][j];
      }
    }
  }

  // === CHOLESKY VERIFICATION: check if L*L^T == A ===
  double maxError = 0.0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      double sum = 0.0;
      for (int k = 0; k < n; k++)
        sum += L[i][k] * L[j][k]; // L^T[k][j] = L[j][k]
      double err = fabs(sum - data[i][j]);
      if (err > maxError)
        maxError = err;
    }
  }
  if (maxError < 1e-6)
    cout << "Cholesky verification PASSED ✅ (max error: " << maxError << ")"
         << endl;
  else
    cout << "Cholesky verification FAILED ⚠️ (max error: " << maxError << ")"
         << endl;

  // forward substitution: Ly = b
  double *y = new double[n];
  for (int i = 0; i < n; i++) {
    double sum = 0.0;
    for (int j = 0; j < i; j++)
      sum += L[i][j] * y[j];
    y[i] = (b[i] - sum) / L[i][i];
  }

  // back substitution: L^T x = y
  double *x = new double[n];
  for (int i = n - 1; i >= 0; i--) {
    double sum = 0.0;
    for (int j = i + 1; j < n; j++)
      sum += L[j][i] * x[j]; // L^T[i][j] = L[j][i]
    x[i] = (y[i] - sum) / L[i][i];
  }

  // cleanup
  for (int i = 0; i < n; i++)
    delete[] L[i];
  delete[] L;
  delete[] y;

  return x;
}
