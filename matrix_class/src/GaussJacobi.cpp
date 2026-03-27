#include "../include/GaussJacobi.hpp"
#include <cmath>
#include <limits>
using namespace std;

GaussJacobi::GaussJacobi() : SystemOfLinearEquationSolver() {}
GaussJacobi::GaussJacobi(int r, int c) : SystemOfLinearEquationSolver(r, c) {}

// Jacobi iterative method
// splits A into D (diagonal) and R (rest)
// x_new[i] = (b[i] - sum(A[i][j]*x_old[j] for j!=i)) / A[i][i]
// uses ONLY old values — stores results in a new array, does NOT overwrite
//
// NO cout here — solver is pure logic. UI layer handles printing.
// Does NOT modify this->data — works on copies.
// Caller is responsible for freeing result.x via delete[].
SolverResult GaussJacobi::solve(double *b, int n, int maxIter, double tol) {
  if (rows != cols || rows != n)
    throw MatrixException("matrix dimensions are sus for Gauss-Jacobi");

  SolverResult result;
  result.n = n;
  result.iterations = 0;
  result.converged = false;
  result.error = 0.0;
  result.dominanceAchieved = true;

  // COPY matrix data and b so we don't mutate the original
  double **A = new double *[n];
  for (int i = 0; i < n; i++) {
    A[i] = new double[n];
    for (int j = 0; j < n; j++)
      A[i][j] = data[i][j];
  }
  double *rhs = new double[n];
  for (int i = 0; i < n; i++)
    rhs[i] = b[i];

  // check diagonal dominance — if not, try to fix by row swapping on copies
  // uses the consolidated base class helper (issue #1)
  // returns bool so we know if dominance was achieved (issue #2, #3)
  if (!isDiagonallyDominant()) {
    result.dominanceAchieved = makeDiagDominant(A, rhs, n);
  }

  // check for zero diagonals
  for (int i = 0; i < n; i++) {
    if (fabs(A[i][i]) < 1e-12) {
      // cleanup everything before throwing (issue #6)
      for (int k = 0; k < n; k++)
        delete[] A[k];
      delete[] A;
      delete[] rhs;
      throw MatrixException(
          "zero on diagonal... Jacobi can't work with this");
    }
  }

  // initial guess x = 0
  double *x_old = new double[n];
  double *x_new = new double[n];
  for (int i = 0; i < n; i++) {
    x_old[i] = 0.0;
    x_new[i] = 0.0;
  }

  // iterate
  double lastMaxDiff = 0.0; // track last diff for result.error (issue #4)
  int iter;
  for (iter = 0; iter < maxIter; iter++) {
    bool diverged = false;

    // compute ALL x_new from x_old (use only old values!)
    for (int i = 0; i < n; i++) {
      double sum = 0.0;
      for (int j = 0; j < n; j++) {
        if (j != i)
          sum += A[i][j] * x_old[j];
      }
      x_new[i] = (rhs[i] - sum) / A[i][i];

      // check for NaN/Inf divergence
      if (isnan(x_new[i]) || isinf(x_new[i])) {
        diverged = true;
        break;
      }
    }

    if (diverged) {
      // set error to infinity on divergence (issue #5)
      result.error = numeric_limits<double>::infinity();
      result.x = x_new;
      result.iterations = iter + 1;
      result.converged = false;
      // cleanup all heap allocations (issue #6)
      delete[] x_old;
      for (int i = 0; i < n; i++)
        delete[] A[i];
      delete[] A;
      delete[] rhs;
      return result;
    }

    // check convergence
    double maxDiff = 0.0;
    for (int i = 0; i < n; i++) {
      double diff = fabs(x_new[i] - x_old[i]);
      if (diff > maxDiff)
        maxDiff = diff;
    }
    lastMaxDiff = maxDiff; // always track (issue #4)

    // copy new to old (replace old vector)
    for (int i = 0; i < n; i++)
      x_old[i] = x_new[i];

    if (maxDiff < tol) {
      result.iterations = iter + 1;
      result.converged = true;
      result.error = maxDiff;
      break;
    }
  }

  if (!result.converged) {
    result.iterations = maxIter;
    result.error = lastMaxDiff; // non-zero error on non-convergence (issue #4)
  }

  result.x = x_new;

  // cleanup
  delete[] x_old;
  for (int i = 0; i < n; i++)
    delete[] A[i];
  delete[] A;
  delete[] rhs;

  return result;
}
