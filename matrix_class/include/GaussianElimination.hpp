#ifndef GAUSSIANELIMINATION_HPP
#define GAUSSIANELIMINATION_HPP

#include "SystemOfLinearEquationSolver.hpp"

class GaussianElimination : public SystemOfLinearEquationSolver {
public:
  GaussianElimination();
  GaussianElimination(int r, int c);

  // default solve uses pivoting (maxIter/tol ignored for direct methods)
  SolverResult solve(double *b, int n,
                     int maxIter = 10000,
                     double tol = 1e-10) override;

  // solve Ax = b — the only thing this class adds
  double *solveWithPivoting(double *b, int n);
  double *solveWithoutPivoting(double *b, int n);
};

#endif
