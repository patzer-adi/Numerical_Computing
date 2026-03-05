#ifndef GAUSSIANELIMINATION_HPP
#define GAUSSIANELIMINATION_HPP

#include "SystemOfLinearEquationSolver.hpp"

class GaussianElimination : public SystemOfLinearEquationSolver {
public:
  GaussianElimination();
  GaussianElimination(int r, int c);

  // default solve uses pivoting
  double *solve(double *b, int n);

  // solve Ax = b — the only thing this class adds
  double *solveWithPivoting(double *b, int n);
  double *solveWithoutPivoting(double *b, int n);
};

#endif
