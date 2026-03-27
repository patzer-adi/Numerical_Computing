#include "../include/SystemOfLinearEquationSolver.hpp"
#include <cmath>

SystemOfLinearEquationSolver::SystemOfLinearEquationSolver() : Matrix() {}
SystemOfLinearEquationSolver::SystemOfLinearEquationSolver(int r, int c)
    : Matrix(r, c) {}

// check if a raw A[][] is diagonally dominant
// static utility — works on any double** array, not on this->data
bool SystemOfLinearEquationSolver::checkDiagDominant(double **A, int n) {
  for (int i = 0; i < n; i++) {
    double diag = fabs(A[i][i]);
    double sum = 0.0;
    for (int j = 0; j < n; j++) {
      if (j != i)
        sum += fabs(A[i][j]);
    }
    if (diag < sum)
      return false;
  }
  return true;
}

// consolidated diagonal dominance row-swap helper
// for each column i, tries to find a row (from i+1 onward) that would be
// strictly dominant at position i. Among qualifying rows, picks the one
// with the largest |A[r][i]| (partial pivoting style — issue #11).
// After all swaps, re-verifies and returns true if dominant, false otherwise.
bool SystemOfLinearEquationSolver::makeDiagDominant(double **A, double *b,
                                                    int n) {
  for (int i = 0; i < n; i++) {
    // check if current row is already dominant at position i
    double diag = fabs(A[i][i]);
    double sum = 0.0;
    for (int j = 0; j < n; j++) {
      if (j != i)
        sum += fabs(A[i][j]);
    }
    if (diag >= sum)
      continue; // already dominant, skip

    // find the BEST row (largest |A[r][i]|) among those that would be dominant
    int bestRow = -1;
    double bestDiag = -1.0;
    for (int r = i + 1; r < n; r++) {
      double d = fabs(A[r][i]);
      double s = 0.0;
      for (int j = 0; j < n; j++) {
        if (j != i)
          s += fabs(A[r][j]);
      }
      if (d >= s && d > bestDiag) {
        bestDiag = d;
        bestRow = r;
      }
    }

    // swap if we found a qualifying row
    if (bestRow != -1) {
      double *tempRow = A[i];
      A[i] = A[bestRow];
      A[bestRow] = tempRow;

      double tempB = b[i];
      b[i] = b[bestRow];
      b[bestRow] = tempB;
    }
  }

  // re-verify dominance after all swaps
  return checkDiagDominant(A, n);
}
