#include "../include/GaussJacobi.hpp"
#include <cmath>
#include <iostream>
using namespace std;

GaussJacobi::GaussJacobi() : SystemOfLinearEquationSolver() {}
GaussJacobi::GaussJacobi(int r, int c) : SystemOfLinearEquationSolver(r, c) {}

// Jacobi iterative method
// splits A into D (diagonal) and R (rest)
// x_new[i] = (b[i] - sum(A[i][j]*x_old[j] for j!=i)) / A[i][i]
double *GaussJacobi::solve(double *b, int n) {
  if (rows != cols || rows != n)
    throw MatrixException("matrix dimensions are sus for Gauss-Jacobi");

  int maxIter = 10000;
  double tol = 1e-10;

  // check diagonal dominance (warn but continue)
  for (int i = 0; i < n; i++) {
    if (fabs(data[i][i]) < 1e-12)
      throw MatrixException(
          "zero on diagonal... Jacobi can't work with this 💀");
  }

  // initial guess x = 0
  double *x_old = new double[n];
  double *x_new = new double[n];
  for (int i = 0; i < n; i++) {
    x_old[i] = 0.0;
    x_new[i] = 0.0;
  }

  // iterate
  int iter;
  for (iter = 0; iter < maxIter; iter++) {
    for (int i = 0; i < n; i++) {
      double sum = 0.0;
      for (int j = 0; j < n; j++) {
        if (j != i)
          sum += data[i][j] * x_old[j];
      }
      x_new[i] = (b[i] - sum) / data[i][i];
    }

    // check convergence
    double maxDiff = 0.0;
    for (int i = 0; i < n; i++) {
      double diff = fabs(x_new[i] - x_old[i]);
      if (diff > maxDiff)
        maxDiff = diff;
    }

    // copy new to old
    for (int i = 0; i < n; i++)
      x_old[i] = x_new[i];

    if (maxDiff < tol) {
      cout << "Gauss-Jacobi converged in " << iter + 1 << " iterations" << endl;
      break;
    }
  }

  if (iter == maxIter)
    cout << "Gauss-Jacobi did NOT converge after " << maxIter
         << " iterations... results might be off" << endl;

  delete[] x_old;
  return x_new;
}
