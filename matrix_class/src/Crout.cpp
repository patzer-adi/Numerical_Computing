#include "../include/LUDecomposition.hpp"
#include <cmath>
using namespace std;

Crout::Crout() : LUDecomposition() {}
Crout::Crout(int r, int c) : LUDecomposition(r, c) {}

SolverResult Crout::solve(double *b, int n, int maxIter, double tol) {
  if (rows != cols || rows != n)
    throw MatrixException("matrix dimensions don't match for Crout LU");

  double **L = new double *[n];
  double **U = new double *[n];
  for (int i = 0; i < n; i++) {
    L[i] = new double[n];
    U[i] = new double[n];
    for (int j = 0; j < n; j++) {
      L[i][j] = 0.0;
      U[i][j] = 0.0;
    }
    U[i][i] = 1.0;
  }

  for (int k = 0; k < n; k++) {
    for (int i = k; i < n; i++) {
      double sum = 0.0;
      for (int s = 0; s < k; s++)
        sum += L[i][s] * U[s][k];
      L[i][k] = data[i][k] - sum;
    }
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

  double *y = new double[n];
  for (int i = 0; i < n; i++) {
    double sum = 0.0;
    for (int j = 0; j < i; j++)
      sum += L[i][j] * y[j];
    if (fabs(L[i][i]) < 1e-12)
      throw MatrixException("zero on L diagonal in forward sub... oof");
    y[i] = (b[i] - sum) / L[i][i];
  }

  double *x = new double[n];
  for (int i = n - 1; i >= 0; i--) {
    double sum = 0.0;
    for (int j = i + 1; j < n; j++)
      sum += U[i][j] * x[j];
    x[i] = y[i] - sum;
  }

  for (int i = 0; i < n; i++) {
    delete[] L[i];
    delete[] U[i];
  }
  delete[] L;
  delete[] U;
  delete[] y;

  SolverResult result;
  result.x = x;
  result.n = n;
  result.iterations = 0;
  result.converged = true;
  result.error = maxError;
  return result;
}
