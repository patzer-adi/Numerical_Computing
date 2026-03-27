#ifndef LUDECOMPOSITION_HPP
#define LUDECOMPOSITION_HPP

#include "SystemOfLinearEquationSolver.hpp"

// abstract base class for LU methods
class LUDecomposition : public SystemOfLinearEquationSolver {
public:
  LUDecomposition();
  LUDecomposition(int r, int c);
  virtual ~LUDecomposition() {}

  // solve Ax = b — each subclass implements this
  virtual SolverResult solve(double *b, int n,
                             int maxIter = 10000,
                             double tol = 1e-10) = 0;
};

// Doolittle method - L has unit diagonal
class Doolittle : public LUDecomposition {
public:
  Doolittle();
  Doolittle(int r, int c);
  SolverResult solve(double *b, int n,
                     int maxIter = 10000,
                     double tol = 1e-10) override;
};

// Crout method - U has unit diagonal
class Crout : public LUDecomposition {
public:
  Crout();
  Crout(int r, int c);
  SolverResult solve(double *b, int n,
                     int maxIter = 10000,
                     double tol = 1e-10) override;
};

// Cholesky method - for symmetric positive definite matrices
class Cholesky : public LUDecomposition {
public:
  Cholesky();
  Cholesky(int r, int c);
  SolverResult solve(double *b, int n,
                     int maxIter = 10000,
                     double tol = 1e-10) override;
};

#endif
