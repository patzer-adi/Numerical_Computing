#include "../include/LUDecomposition.hpp"
#include <cmath>

#ifdef USE_CUDA
#include "../cuda/include/gpu_backend.cuh"
#include "../cuda/include/gpu_dispatch.cuh"
#endif

using namespace std;

// Doolittle constructors
Doolittle::Doolittle() : LUDecomposition() {}
Doolittle::Doolittle(int r, int c) : LUDecomposition(r, c) {}

// Doolittle LU decomposition
// L has 1s on diagonal, U is upper triangular
SolverResult Doolittle::solve(double *b, int n, int maxIter, double tol) {
  if (rows != cols || rows != n)
    throw MatrixException("matrix dimensions don't match for Doolittle LU");

  double **L = new double *[n];
  double **U = new double *[n];
  for (int i = 0; i < n; i++) {
    L[i] = new double[n];
    U[i] = new double[n];
    for (int j = 0; j < n; j++) {
      L[i][j] = 0.0;
      U[i][j] = 0.0;
    }
    L[i][i] = 1.0;
  }

#ifdef USE_CUDA
  if (BackendDispatcher::shouldUseGPU(n, "lu_factorize")) {
    double *flatA = new double[n * n];
    double *flatL = new double[n * n];
    double *flatU = new double[n * n];
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++) {
        flatA[i * n + j] = data[i][j];
        flatL[i * n + j] = L[i][j];
        flatU[i * n + j] = U[i][j];
      }
    gpuLU_Doolittle(flatA, flatL, flatU, n);
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++) {
        L[i][j] = flatL[i * n + j];
        U[i][j] = flatU[i * n + j];
      }
    delete[] flatA;
    delete[] flatL;
    delete[] flatU;
  } else {
#endif
    for (int k = 0; k < n; k++) {
      for (int j = k; j < n; j++) {
        double sum = 0.0;
        for (int s = 0; s < k; s++)
          sum += L[k][s] * U[s][j];
        U[k][j] = data[k][j] - sum;
      }
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
#ifdef USE_CUDA
  }
#endif

  // LU verification: compute max |L*U - A|
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
    y[i] = b[i] - sum;
  }

  double *x = new double[n];
  for (int i = n - 1; i >= 0; i--) {
    double sum = 0.0;
    for (int j = i + 1; j < n; j++)
      sum += U[i][j] * x[j];
    if (fabs(U[i][i]) < 1e-12)
      throw MatrixException("zero on U diagonal in back sub... rip");
    x[i] = (y[i] - sum) / U[i][i];
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
  result.dominanceAchieved = true;
  result.error = maxError;
  return result;
}
