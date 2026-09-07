#ifndef NEWTONDIVIDEDDIFFERENCE_HPP
#define NEWTONDIVIDEDDIFFERENCE_HPP

#include "Interpolation.hpp"

// NewtonDividedDifference — Newton's Divided Difference interpolation
//
// Builds a divided difference table from n data points and evaluates
// the interpolating polynomial using Horner's nested form:
//
//   P(x) = f[x₀] + f[x₀,x₁](x−x₀) + f[x₀,x₁,x₂](x−x₀)(x−x₁) + ⋯
//
// The divided difference table is an n×n lower-triangular Matrix:
//   Column 0: f[xᵢ] = yᵢ
//   Column j: f[xᵢ, ..., xᵢ₊ⱼ] = (f[xᵢ₊₁,...,xᵢ₊ⱼ] − f[xᵢ,...,xᵢ₊ⱼ₋₁]) / (xᵢ₊ⱼ − xᵢ)
//
// Uses Matrix class to store the divided difference table (composition).

class NewtonDividedDifference : public Interpolation {
protected:
  mutable Matrix ddTable;    // n×n divided difference table (mutable for lazy const eval)
  mutable bool computed;     // have we built the table?

  // build the divided difference table from data points
  void buildTable() const;

public:
  // constructors
  NewtonDividedDifference();
  NewtonDividedDifference(const Matrix &x, const Matrix &y);

  // evaluate the Newton polynomial at a single x using Horner's form
  double evaluate(double x) const override;

  // print the divided difference table and polynomial coefficients
  void printFitInfo() override;

  // get a specific divided difference coefficient f[x₀, ..., xₖ]
  // k = 0 returns f[x₀], k = 1 returns f[x₀, x₁], etc.
  double getCoefficient(int k) const;

  // get the full divided difference table as a Matrix
  Matrix getTable() const;
};

#endif
