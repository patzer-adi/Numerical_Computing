#include "../include/Lagrange.hpp"
#include <cmath>
using namespace std;

// default constructor
Lagrange::Lagrange() : Interpolation() {}

// parameterized constructor — takes Matrix objects
Lagrange::Lagrange(const Matrix &x, const Matrix &y) : Interpolation(x, y) {}

// evaluate the Lagrange polynomial at a single x
double Lagrange::evaluate(double x) const {
  // check for duplicate x values — would cause division by zero
  for (int i = 0; i < numPoints; i++) {
    for (int j = i + 1; j < numPoints; j++) {
      if (fabs(getX(i) - getX(j)) < 1e-10)
        throw MatrixException(
            "duplicate x values found... Lagrange can't handle that "
            "(division by zero)");
    }
  }

  double result = 0.0;

  for (int i = 0; i < numPoints; i++) {
    double xi = getX(i);
    double yi = getY(i);

    // compute the i-th Lagrange basis polynomial L_i(x)
    double Li = 1.0;
    for (int j = 0; j < numPoints; j++) {
      if (j != i) {
        Li *= (x - getX(j)) / (xi - getX(j));
      }
    }
    result += yi * Li;
  }

  return result;
}
