#include "../include/NewtonDividedDifference.hpp"
#include <cmath>
#include <iomanip>
#include <iostream>
using namespace std;

// default constructor
NewtonDividedDifference::NewtonDividedDifference()
    : Interpolation(), computed(false) {}

// parameterized constructor — takes Matrix objects
NewtonDividedDifference::NewtonDividedDifference(const Matrix &x, const Matrix &y)
    : Interpolation(x, y), computed(false) {}

// build the divided difference table
// stores as an n×n lower-triangular Matrix where:
//   ddTable(i, 0) = f[xᵢ] = yᵢ
//   ddTable(i, j) = f[xᵢ₋ⱼ, ..., xᵢ] for j >= 1
//
// the diagonal elements ddTable(k, k) are NOT the polynomial coefficients —
// the coefficients are ddTable(j, j) which equals f[x₀, x₁, ..., xⱼ]
void NewtonDividedDifference::buildTable() const {
  if (computed) return;

  if (numPoints < 2)
    throw MatrixException(
        "Newton divided difference needs at least 2 data points");

  // check for duplicate x values — would cause division by zero
  for (int i = 0; i < numPoints; i++) {
    for (int j = i + 1; j < numPoints; j++) {
      if (fabs(getX(i) - getX(j)) < 1e-10)
        throw MatrixException(
            "duplicate x values found... Newton divided difference can't "
            "handle that (division by zero)");
    }
  }

  int n = numPoints;

  // create n×n table initialized to 0
  ddTable = Matrix(n, n);

  // column 0: zeroth-order divided differences = y values
  for (int i = 0; i < n; i++) {
    ddTable.setData(i, 0, getY(i));
  }

  // fill columns 1 to n-1: higher-order divided differences
  for (int j = 1; j < n; j++) {
    for (int i = j; i < n; i++) {
      double num = ddTable.getData(i, j - 1) - ddTable.getData(i - 1, j - 1);
      double den = getX(i) - getX(i - j);
      ddTable.setData(i, j, num / den);
    }
  }

  computed = true;
}

// evaluate the Newton polynomial at a single x using Horner's nested form
//
// P(x) = f[x₀] + (x-x₀)(f[x₀,x₁] + (x-x₁)(f[x₀,x₁,x₂] + ...))
//
// working from the inside out (highest order to lowest):
//   result = coeff[n-1]
//   for i = n-2 down to 0:
//       result = result * (x - x[i]) + coeff[i]
double NewtonDividedDifference::evaluate(double x) const {
  if (!computed) buildTable();

  int n = numPoints;

  // coefficients are the diagonal elements: ddTable(k, k) = f[x₀, ..., xₖ]
  // Horner's form: start from the last coefficient and work backwards
  double result = ddTable.getData(n - 1, n - 1);

  for (int i = n - 2; i >= 0; i--) {
    result = result * (x - getX(i)) + ddTable.getData(i, i);
  }

  return result;
}

// print the divided difference table and polynomial coefficients
void NewtonDividedDifference::printFitInfo() {
  if (!computed) buildTable();

  int n = numPoints;

  cout << fixed << setprecision(6);
  cout << "\n--- Newton Divided Difference Table ---" << endl;

  // header row
  cout << left << setw(12) << "x";
  cout << setw(14) << "f[.]";
  for (int j = 1; j < n; j++) {
    string header = "f[";
    for (int k = 0; k <= j; k++) {
      if (k > 0) header += ",";
      header += ".";
    }
    header += "]";
    cout << setw(14) << header;
  }
  cout << endl;
  cout << string(12 + 14 * n, '-') << endl;

  // table rows
  for (int i = 0; i < n; i++) {
    cout << left << setw(12) << getX(i);
    for (int j = 0; j <= i; j++) {
      cout << setw(14) << ddTable.getData(i, j);
    }
    cout << endl;
  }

  // polynomial coefficients (diagonal elements)
  cout << "\n--- Polynomial Coefficients ---" << endl;
  for (int k = 0; k < n; k++) {
    cout << "  f[x₀";
    for (int j = 1; j <= k; j++) {
      cout << ", x" << j;
    }
    cout << "] = " << ddTable.getData(k, k) << endl;
  }

  // polynomial expression
  cout << "\n  P(x) = " << ddTable.getData(0, 0);
  for (int k = 1; k < n; k++) {
    double coeff = ddTable.getData(k, k);
    if (coeff >= 0)
      cout << " + " << coeff;
    else
      cout << " - " << fabs(coeff);
    for (int j = 0; j < k; j++) {
      double xj = getX(j);
      if (xj >= 0)
        cout << "(x - " << xj << ")";
      else
        cout << "(x + " << fabs(xj) << ")";
    }
  }
  cout << endl;
}

// get a specific divided difference coefficient f[x₀, ..., xₖ]
double NewtonDividedDifference::getCoefficient(int k) const {
  if (!computed) buildTable();

  if (k < 0 || k >= numPoints)
    throw MatrixException("coefficient index out of bounds in getCoefficient()");

  return ddTable.getData(k, k);
}

// get the full divided difference table as a Matrix
Matrix NewtonDividedDifference::getTable() const {
  if (!computed) buildTable();
  return ddTable;
}
