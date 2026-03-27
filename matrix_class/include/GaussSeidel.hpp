#ifndef GAUSSSEIDEL_HPP
#define GAUSSSEIDEL_HPP

#include "SystemOfLinearEquationSolver.hpp"

class GaussSeidel : public SystemOfLinearEquationSolver {
public:
  GaussSeidel();
  GaussSeidel(int r, int c);

  // solve Ax = b using Gauss-Seidel iteration
  // maxIter and tol are configurable (default: 10000, 1e-10)
  SolverResult solve(double *b, int n,
                     int maxIter = 10000,
                     double tol = 1e-10) override;
};

#endif
