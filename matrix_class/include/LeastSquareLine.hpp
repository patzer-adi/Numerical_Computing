#ifndef LEASTSQUARELINE_HPP
#define LEASTSQUARELINE_HPP

#include "Interpolation.hpp"

// LeastSquareLine — Least Squares Straight Line Fit
//
// Fits y = a + bx to n data points by minimizing
// the sum of squared errors (SSE).
//
// Normal equations (solved using Matrix inverse):
//   Σy   = n·a + b·Σx
//   Σxy  = a·Σx + b·Σx²
//
// Uses Matrix class to build the 2×2 coefficient matrix
// and 2×1 RHS vector, then solves via A.inverse() * rhs.

class LeastSquareLine : public Interpolation {
protected:
  mutable double coeffA, coeffB; // y = a + bx (mutable for lazy const eval)
  mutable bool fitted;           // have we computed coefficients?

  // compute coefficients from normal equations using Matrix operations
  void fit() const;

public:
  // constructors
  LeastSquareLine();
  LeastSquareLine(const Matrix &x, const Matrix &y);

  // evaluate the fitted line at a single x
  double evaluate(double x) const override;

  // print the fitted equation and coefficients
  void printFitInfo() override;

  // error analysis
  bool hasErrorAnalysis() const override;
  void printErrorTable() const override;
  void saveErrorTable(string filename) const override;

  // coefficient getters
  double getA() const;
  double getB() const;
};

#endif
