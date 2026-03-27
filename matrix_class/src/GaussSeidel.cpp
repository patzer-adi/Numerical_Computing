#include "../include/GaussSeidel.hpp"
#include <cmath>
#include <limits>
using namespace std;

GaussSeidel::GaussSeidel() : SystemOfLinearEquationSolver() {}
GaussSeidel::GaussSeidel(int r, int c) : SystemOfLinearEquationSolver(r, c) {}

// Gauss-Seidel iterative method
// like Jacobi, but uses updated x values as soon as they're computed
// x[i] = (b[i] - sum(A[i][j]*x[j] for j<i, using NEW x)
//                - sum(A[i][j]*x[j] for j>i, using OLD x)) / A[i][i]
//
// NO cout here — solver is pure logic. UI layer handles printing.
// Does NOT modify this->data — works on copies.
// Caller is responsible for freeing result.x via delete[].
SolverResult GaussSeidel::solve(double *b, int n, int maxIter, double tol) {
  if (rows != cols || rows != n)
    throw MatrixException("matrix dimensions are sus for Gauss-Seidel");

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
      for (int k = 0; k < n; k++)
        delete[] A[k];
      delete[] A;
      delete[] rhs;
      throw MatrixException(
          "zero on diagonal... Gauss-Seidel can't work with this");
    }
  }

  // initial guess x = 0
  double *x = new double[n];
  for (int i = 0; i < n; i++)
    x[i] = 0.0;

  // iterate
  double lastMaxDiff = 0.0; // track last diff for result.error (issue #4)
  int iter;
  for (iter = 0; iter < maxIter; iter++) {
    double maxDiff = 0.0;
    bool diverged = false;

    for (int i = 0; i < n; i++) {
      double sum = 0.0;

      // use already-updated x[j] for j < i (this is the Seidel difference!)
      for (int j = 0; j < i; j++)
        sum += A[i][j] * x[j];

      // use old x[j] for j > i
      for (int j = i + 1; j < n; j++)
        sum += A[i][j] * x[j];

      double newVal = (rhs[i] - sum) / A[i][i];

      // check for NaN/Inf divergence
      if (isnan(newVal) || isinf(newVal)) {
        diverged = true;
        break;
      }

      // diff computed BEFORE x[i] = newVal — this is CORRECT, do not change
      double diff = fabs(newVal - x[i]);
      if (diff > maxDiff)
        maxDiff = diff;

      x[i] = newVal; // update immediately (Seidel style)
    }

    lastMaxDiff = maxDiff; // always track (issue #4)

    if (diverged) {
      // set error to infinity on divergence (issue #5)
      result.error = numeric_limits<double>::infinity();
      result.x = x;
      result.iterations = iter + 1;
      result.converged = false;
      for (int i = 0; i < n; i++)
        delete[] A[i];
      delete[] A;
      delete[] rhs;
      return result;
    }

    if (maxDiff < tol) {
      result.iterations = iter + 1;
      result.converged = true;
      result.error = maxDiff;
      break;
    }
  }

  if (!result.converged && iter == maxIter) {
    result.iterations = maxIter;
    result.error = lastMaxDiff; // non-zero error on non-convergence (issue #4)
  }

  result.x = x;

  // cleanup
  for (int i = 0; i < n; i++)
    delete[] A[i];
  delete[] A;
  delete[] rhs;

  return result;
}
