#ifndef GAUSSJACOBI_HPP
#define GAUSSJACOBI_HPP

#include "SystemOfLinearEquationSolver.hpp"

class GaussJacobi : public SystemOfLinearEquationSolver {
public:
  GaussJacobi();
  GaussJacobi(int r, int c);

  // solve Ax = b using Jacobi iteration
  double *solve(double *b, int n);
};

#endif
