#include "../include/LUDecomposition.hpp"
#include <cmath>
#include <iostream>
using namespace std;

// Crout constructors
Crout::Crout() : LUDecomposition() {}
Crout::Crout(int r, int c) : LUDecomposition(r, c) {}

// Crout LU decomposition
// U has 1s on diagonal, L is lower triangular
// Decompose A into L*U, then solve Ly = b (forward sub), then Ux = y (back sub)
double *Crout::solve(double *b, int n) {
  if (rows != cols || rows != n)
    throw MatrixException("matrix dimensions don't match for Crout LU");

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
    U[i][i] = 1.0; // U has 1s on diagonal
  }

  // decompose A = L * U
  for (int k = 0; k < n; k++) {
    // compute L column k
    for (int i = k; i < n; i++) {
      double sum = 0.0;
      for (int s = 0; s < k; s++)
        sum += L[i][s] * U[s][k];
      L[i][k] = data[i][k] - sum;
    }

    // compute U row k
    for (int j = k + 1; j < n; j++) {
      double sum = 0.0;
      for (int s = 0; s < k; s++)
        sum += L[k][s] * U[s][j];
      if (fabs(L[k][k]) < 1e-12)
        throw MatrixException(
            "zero pivot in Crout... matrix might be singular bro");
      U[k][j] = (data[k][j] - sum) / L[k][k];
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
    cout << "Crout LU verification PASSED ✅ (max error: " << maxError << ")"
         << endl;
  else
    cout << "Crout LU verification FAILED ⚠️ (max error: " << maxError << ")"
         << endl;

  // forward substitution: Ly = b
  double *y = new double[n];
  for (int i = 0; i < n; i++) {
    double sum = 0.0;
    for (int j = 0; j < i; j++)
      sum += L[i][j] * y[j];
    if (fabs(L[i][i]) < 1e-12)
      throw MatrixException("zero on L diagonal in forward sub... oof");
    y[i] = (b[i] - sum) / L[i][i];
  }

  // back substitution: Ux = y
  double *x = new double[n];
  for (int i = n - 1; i >= 0; i--) {
    double sum = 0.0;
    for (int j = i + 1; j < n; j++)
      sum += U[i][j] * x[j];
    x[i] = y[i] - sum; // U[i][i] = 1
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
