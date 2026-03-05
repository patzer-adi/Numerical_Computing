#include "../include/LUDecomposition.hpp"
#include <cmath>
#include <iostream>
using namespace std;

// Doolittle constructors
Doolittle::Doolittle() : LUDecomposition() {}
Doolittle::Doolittle(int r, int c) : LUDecomposition(r, c) {}

// Doolittle LU decomposition
// L has 1s on diagonal, U is upper triangular
// Decompose A into L*U, then solve Ly = b (forward sub), then Ux = y (back sub)
double *Doolittle::solve(double *b, int n) {
  if (rows != cols || rows != n)
    throw MatrixException("matrix dimensions don't match for Doolittle LU");

  // allocate L and U
  double **L = new double *[n];
  double **U = new double *[n];
  for (int i = 0; i < n; i++) {
    L[i] = new double[n];
    U[i] = new double[n];
    for (int j = 0; j < n; j++) {
      L[i][j] = 0.0;
      U[i][j] = 0.0;
    }
    L[i][i] = 1.0; // L has 1s on diagonal
  }

  // decompose A = L * U
  for (int k = 0; k < n; k++) {
    // compute U row k
    for (int j = k; j < n; j++) {
      double sum = 0.0;
      for (int s = 0; s < k; s++)
        sum += L[k][s] * U[s][j];
      U[k][j] = data[k][j] - sum;
    }

    // compute L column k
    for (int i = k + 1; i < n; i++) {
      double sum = 0.0;
      for (int s = 0; s < k; s++)
        sum += L[i][s] * U[s][k];
      if (fabs(U[k][k]) < 1e-12)
        throw MatrixException(
            "zero pivot in Doolittle... matrix might be singular bro");
      L[i][k] = (data[i][k] - sum) / U[k][k];
    }
  }

  // === LU VERIFICATION: check if L*U == A ===
  double maxError = 0.0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      double sum = 0.0;
      for (int k = 0; k < n; k++)
        sum += L[i][k] * U[k][j];
      double err = fabs(sum - data[i][j]);
      if (err > maxError)
        maxError = err;
    }
  }
  if (maxError < 1e-6)
    cout << "Doolittle LU verification PASSED ✅ (max error: " << maxError
         << ")" << endl;
  else
    cout << "Doolittle LU verification FAILED ⚠️ (max error: " << maxError << ")"
         << endl;

  // forward substitution: Ly = b
  double *y = new double[n];
  for (int i = 0; i < n; i++) {
    double sum = 0.0;
    for (int j = 0; j < i; j++)
      sum += L[i][j] * y[j];
    y[i] = b[i] - sum; // L[i][i] = 1
  }

  // back substitution: Ux = y
  double *x = new double[n];
  for (int i = n - 1; i >= 0; i--) {
    double sum = 0.0;
    for (int j = i + 1; j < n; j++)
      sum += U[i][j] * x[j];
    if (fabs(U[i][i]) < 1e-12)
      throw MatrixException("zero on U diagonal in back sub... rip");
    x[i] = (y[i] - sum) / U[i][i];
  }

  // cleanup
  for (int i = 0; i < n; i++) {
    delete[] L[i];
    delete[] U[i];
  }
  delete[] L;
  delete[] U;
  delete[] y;

  return x;
}
