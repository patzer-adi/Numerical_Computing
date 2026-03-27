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
};

#endif
