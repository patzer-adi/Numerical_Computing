#ifndef GAUSSIANELIMINATION_HPP
#define GAUSSIANELIMINATION_HPP

#include "Matrix.hpp"

class GaussianElimination : public Matrix {
public:
  GaussianElimination();
  GaussianElimination(int r, int c);

  // solve Ax = b — the only thing this class adds
  double *solveWithPivoting(double *b, int n);
  double *solveWithoutPivoting(double *b, int n);
};

#endif
