#ifndef SOLVERRESULT_HPP
#define SOLVERRESULT_HPP

// SolverResult: what every solver returns
// instead of just a raw pointer, we return useful info about the solve
//
// WHY: returning just double* tells you nothing about HOW the solve went.
// did it converge? how many iterations? what was the error?
// SolverResult answers all of that.
//
// WHO FREES x? the caller (whoever receives this result)

struct SolverResult {
  double *x;             // solution vector (caller must delete[])
  int n;                  // size of x
  int iterations;         // how many iterations (0 for direct methods like GE/LU)
  bool converged;         // did it converge? (always true for direct methods)
  double error;           // max residual or max diff (0.0 if not computed)
  bool dominanceAchieved; // was diagonal dominance achieved? (true for direct methods)

  // LU decomposition matrices (only set by LU solvers, nullptr otherwise)
  double **L;             // lower triangular (caller must freeLU())
  double **U;             // upper triangular (caller must freeLU())
  int luSize;             // size of L and U (0 if not set)

  // initialize LU fields to safe defaults
  SolverResult() : x(nullptr), n(0), iterations(0), converged(false),
                   error(0.0), dominanceAchieved(false),
                   L(nullptr), U(nullptr), luSize(0) {}

  // free L and U memory
  void freeLU() {
    if (L) {
      for (int i = 0; i < luSize; i++) delete[] L[i];
      delete[] L;
      L = nullptr;
    }
    if (U) {
      for (int i = 0; i < luSize; i++) delete[] U[i];
      delete[] U;
      U = nullptr;
    }
    luSize = 0;
  }
};

#endif
