#ifndef LUDECOMPOSITION_HPP
#define LUDECOMPOSITION_HPP

#include "Matrix.hpp"

// abstract base class for LU methods
class LUDecomposition : public Matrix {
public:
  LUDecomposition();
  LUDecomposition(int r, int c);
  virtual ~LUDecomposition() {}

  // solve Ax = b — each subclass implements this
  virtual double *solve(double *b, int n) = 0;
};

// Doolittle method - L has unit diagonal
class Doolittle : public LUDecomposition {
public:
  Doolittle();
  Doolittle(int r, int c);
  double *solve(double *b, int n);
};

// Crout method - U has unit diagonal
class Crout : public LUDecomposition {
public:
  Crout();
  Crout(int r, int c);
  double *solve(double *b, int n);
};

// Cholesky method - for symmetric positive definite matrices
class Cholesky : public LUDecomposition {
public:
  Cholesky();
  Cholesky(int r, int c);
  double *solve(double *b, int n);
};

#endif
