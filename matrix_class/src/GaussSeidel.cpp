#include "../include/GaussSeidel.hpp"
#include <cmath>
using namespace std;

GaussSeidel::GaussSeidel() : SystemOfLinearEquationSolver() {}
GaussSeidel::GaussSeidel(int r, int c) : SystemOfLinearEquationSolver(r, c) {}

// helper: make A diagonally dominant, rearranging rows of both A and b together
// for each column i, find a row r where |a[r][i]| >= sum of |a[r][j]| for j!=i
static bool makeDiagDominant(double **A, double *b, int n) {
  for (int i = 0; i < n; i++) {
    double diag = fabs(A[i][i]);
    double sum = 0.0;
    for (int j = 0; j < n; j++) {
      if (j != i)
        sum += fabs(A[i][j]);
    }
    if (diag >= sum)
      continue;

    bool found = false;
    for (int r = i + 1; r < n; r++) {
      double d = fabs(A[r][i]);
      double s = 0.0;
      for (int j = 0; j < n; j++) {
        if (j != i)
          s += fabs(A[r][j]);
      }

      if (d >= s) {
        double *tempRow = A[i];
        A[i] = A[r];
        A[r] = tempRow;

        double tempB = b[i];
        b[i] = b[r];
        b[r] = tempB;

        found = true;
        break;
      }
    }

    if (!found)
      return false;
  }
  return true;
}

// Gauss-Seidel iterative method
// like Jacobi, but uses updated x values as soon as they're computed
// x[i] = (b[i] - sum(A[i][j]*x[j] for j<i, using NEW x)
//                - sum(A[i][j]*x[j] for j>i, using OLD x)) / A[i][i]
//
// NO cout here — solver is pure logic. UI layer handles printing.
// Does NOT modify this->data — works on copies.
SolverResult GaussSeidel::solve(double *b, int n, int maxIter, double tol) {
  if (rows != cols || rows != n)
    throw MatrixException("matrix dimensions are sus for Gauss-Seidel");

  SolverResult result;
  result.n = n;
  result.iterations = 0;
  result.converged = false;
  result.error = 0.0;

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
  if (!isDiagonallyDominant()) {
    makeDiagDominant(A, rhs, n);
  }

  // check for zero diagonals
  for (int i = 0; i < n; i++) {
    if (fabs(A[i][i]) < 1e-12) {
      for (int k = 0; k < n; k++) delete[] A[k];
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

      double diff = fabs(newVal - x[i]);
      if (diff > maxDiff)
        maxDiff = diff;

      x[i] = newVal; // update immediately (Seidel style)
    }

    if (diverged) {
      result.x = x;
      result.iterations = iter + 1;
      result.converged = false;
      for (int i = 0; i < n; i++) delete[] A[i];
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
  }

  result.x = x;

  // cleanup
  for (int i = 0; i < n; i++) delete[] A[i];
  delete[] A;
  delete[] rhs;

  return result;
}
