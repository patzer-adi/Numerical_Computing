#ifndef SYSTEMOFLINEAREQUATIONSOLVER_HPP
#define SYSTEMOFLINEAREQUATIONSOLVER_HPP

#include "Matrix.hpp"
#include "SolverResult.hpp"

// abstract base class for all solvers (GE, LU, Jacobi, Seidel)
// every solver must implement solve()
class SystemOfLinearEquationSolver : public Matrix {
public:
  SystemOfLinearEquationSolver();
  SystemOfLinearEquationSolver(int r, int c);
  virtual ~SystemOfLinearEquationSolver() {}

  // every solver class must override this
  // returns a SolverResult with x, iterations, convergence info
  virtual SolverResult solve(double *b, int n,
                             int maxIter = 10000,
                             double tol = 1e-10) = 0;

protected:
  // consolidated diagonal dominance helper — shared by all iterative solvers
  // rearranges rows of A and b to achieve diagonal dominance where possible
  // returns true if the resulting matrix IS diagonally dominant
  static bool makeDiagDominant(double **A, double *b, int n);

  // check if a raw A[][] is diagonally dominant (operates on copy, not this->data)
  static bool checkDiagDominant(double **A, int n);
};

#endif
