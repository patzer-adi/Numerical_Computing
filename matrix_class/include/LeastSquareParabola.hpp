#ifndef LEASTSQUAREPARABOLA_HPP
#define LEASTSQUAREPARABOLA_HPP

#include "Interpolation.hpp"

// LeastSquareParabola — Least Squares Parabolic (Second Degree) Fit
//
// Fits y = a + bx + cx² to n data points by minimizing
// the sum of squared errors (SSE).
//
// Normal equations (solved using Matrix inverse):
//   Σy    = n·a  + b·Σx  + c·Σx²
//   Σxy   = a·Σx  + b·Σx² + c·Σx³
//   Σx²y  = a·Σx² + b·Σx³ + c·Σx⁴
//
// Uses Matrix class to build the 3×3 coefficient matrix
// and 3×1 RHS vector, then solves via A.inverse() * rhs.

class LeastSquareParabola : public Interpolation {
protected:
  mutable double coeffA, coeffB, coeffC; // y = a + bx + cx² (mutable for lazy const eval)
  mutable bool fitted;                   // have we computed coefficients?

  // compute coefficients from normal equations using Matrix operations
  void fit() const;

public:
  // constructors
  LeastSquareParabola();
  LeastSquareParabola(const Matrix &x, const Matrix &y);

  // evaluate the fitted parabola at a single x
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
  double getC() const;
};

#endif
