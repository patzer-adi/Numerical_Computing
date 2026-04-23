#ifndef LAGRANGE_HPP
#define LAGRANGE_HPP

#include "Interpolation.hpp"

// Lagrange — Lagrange interpolation method
//
// Implements the Lagrange interpolation formula:
//   P(x) = sum_{i=0}^{n-1} y_i * L_i(x)
// where
//   L_i(x) = product_{j=0, j!=i}^{n-1} (x - x_j) / (x_i - x_j)

class Lagrange : public Interpolation {
public:
  Lagrange();
  Lagrange(const Matrix &x, const Matrix &y);

  // evaluate the Lagrange polynomial at a single x
  double evaluate(double x) const override;
};

#endif
