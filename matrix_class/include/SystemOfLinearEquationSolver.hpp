#ifndef SYSTEMOFLINEAREQUATIONSOLVER_HPP
#define SYSTEMOFLINEAREQUATIONSOLVER_HPP

#include "Matrix.hpp"

// abstract base class for all solvers (GE, LU, GJ)
// every solver must implement solve()
class SystemOfLinearEquationSolver : public Matrix {
public:
  SystemOfLinearEquationSolver();
  SystemOfLinearEquationSolver(int r, int c);
  virtual ~SystemOfLinearEquationSolver() {}

  // every solver class must override this
  virtual double *solve(double *b, int n) = 0;
};

#endif
