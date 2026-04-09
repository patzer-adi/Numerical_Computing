#include "../include/Lagrange.hpp"
using namespace std;

// default constructor
Lagrange::Lagrange() : Interpolation() {}

// parameterized constructor
Lagrange::Lagrange(double *x, double *y, int n) : Interpolation(x, y, n) {}

// evaluate the Lagrange polynomial at a single x
//
// formula:
//   P(x) = sum_{i=0}^{n-1} y_i * L_i(x)
//   L_i(x) = product_{j=0, j!=i}^{n-1} (x - x_j) / (x_i - x_j)
//
double Lagrange::evaluate(double x) {
  double result = 0.0;

  for (int i = 0; i < numPoints; i++) {
    // compute the i-th Lagrange basis polynomial L_i(x)
    double Li = 1.0;
    for (int j = 0; j < numPoints; j++) {
      if (j != i) {
        Li *= (x - xData[j]) / (xData[i] - xData[j]);
      }
    }
    result += yData[i] * Li;
  }

  return result;
}
