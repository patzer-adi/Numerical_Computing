#include "../include/LeastSquareParabola.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

// default constructor
LeastSquareParabola::LeastSquareParabola()
    : Interpolation(), coeffA(0.0), coeffB(0.0), coeffC(0.0), fitted(false) {}

// parameterized constructor — takes Matrix objects
LeastSquareParabola::LeastSquareParabola(const Matrix &x, const Matrix &y)
    : Interpolation(x, y), coeffA(0.0), coeffB(0.0), coeffC(0.0),
      fitted(false) {}

// compute coefficients using normal equations via Matrix class
//
// normal equations for y = a + bx + cx²:
//   [n      Σx     Σx² ] [a]   [Σy   ]
//   [Σx     Σx²    Σx³ ] [b] = [Σxy  ]
//   [Σx²    Σx³    Σx⁴ ] [c]   [Σx²y ]
//
// solved as: coefficients = A^(-1) * rhs
//
void LeastSquareParabola::fit() const {
  if (fitted) return;

  if (numPoints < 3)
    throw MatrixException(
        "least squares parabola needs at least 3 data points... "
        "can't fit a parabola with less");

  // step 1: compute required sums from data (stored in Matrix xData, yData)
  double sumX = 0.0, sumY = 0.0;
  double sumX2 = 0.0, sumX3 = 0.0, sumX4 = 0.0;
  double sumXY = 0.0, sumX2Y = 0.0;
  int n = numPoints;

  for (int i = 0; i < n; i++) {
    double xi = getX(i);
    double yi = getY(i);
    double xi2 = xi * xi;
    double xi3 = xi2 * xi;
    double xi4 = xi3 * xi;

    sumX += xi;
    sumY += yi;
    sumX2 += xi2;
    sumX3 += xi3;
    sumX4 += xi4;
    sumXY += xi * yi;
    sumX2Y += xi2 * yi;
  }

  // step 2: build the 3×3 coefficient matrix using Matrix class
  Matrix A(3, 3);
  A.setData(0, 0, (double)n);
  A.setData(0, 1, sumX);
  A.setData(0, 2, sumX2);
  A.setData(1, 0, sumX);
  A.setData(1, 1, sumX2);
  A.setData(1, 2, sumX3);
  A.setData(2, 0, sumX2);
  A.setData(2, 1, sumX3);
  A.setData(2, 2, sumX4);

  // step 3: build the 3×1 RHS vector using Matrix class
  Matrix rhs(3, 1);
  rhs.setData(0, 0, sumY);
  rhs.setData(1, 0, sumXY);
  rhs.setData(2, 0, sumX2Y);

  // step 4: solve using Matrix inverse and multiplication
  // coefficients = A^(-1) * rhs
  Matrix Ainv = A.inverse();
  Matrix coeffs = Ainv * rhs;

  // step 5: extract coefficients from result Matrix
  coeffA = coeffs.getData(0, 0);
  coeffB = coeffs.getData(1, 0);
  coeffC = coeffs.getData(2, 0);
  fitted = true;
}

// evaluate the fitted parabola at a single x
// y = a + bx + cx²
double LeastSquareParabola::evaluate(double x) const {
  if (!fitted) fit();
  return coeffA + coeffB * x + coeffC * x * x;
}

// print the fitted equation and coefficients
void LeastSquareParabola::printFitInfo() {
  if (!fitted) fit();

  cout << fixed << setprecision(6);
  cout << "\n--- Least Squares Parabola Fit ---" << endl;
  cout << "Best fit parabola: y = a + bx + cx^2" << endl;
  cout << "  a (constant)    = " << coeffA << endl;
  cout << "  b (linear)      = " << coeffB << endl;
  cout << "  c (quadratic)   = " << coeffC << endl;

  cout << "\n  y = " << coeffA;
  if (coeffB >= 0)
    cout << " + " << coeffB << "x";
  else
    cout << " - " << fabs(coeffB) << "x";
  if (coeffC >= 0)
    cout << " + " << coeffC << "x^2";
  else
    cout << " - " << fabs(coeffC) << "x^2";
  cout << endl;
}

// error analysis support
bool LeastSquareParabola::hasErrorAnalysis() const { return true; }

// print error analysis table — computed on-the-fly from coefficients
void LeastSquareParabola::printErrorTable() const {
  if (!fitted) fit();

  cout << fixed << setprecision(6);
  cout << "\n--- Error Analysis Table (Least Squares Parabola) ---" << endl;
  cout << left << setw(12) << "x"
       << setw(14) << "y(actual)"
       << setw(14) << "y(pred)"
       << setw(14) << "error"
       << setw(14) << "error^2" << endl;
  cout << string(68, '-') << endl;

  double sse = 0.0;
  for (int i = 0; i < numPoints; i++) {
    double xi = getX(i);
    double yActual = getY(i);
    double yPred = evaluate(xi);
    double err = yActual - yPred;
    double err2 = err * err;
    sse += err2;

    cout << left << setw(12) << xi
         << setw(14) << yActual
         << setw(14) << yPred
         << setw(14) << err
         << setw(14) << err2 << endl;
  }

  cout << "\nTotal SSE (Sum of Squared Errors): " << sse << endl;
  cout << "MSE  (Mean Squared Error):         " << sse / numPoints << endl;
  cout << "RMSE (Root Mean Squared Error):    " << sqrt(sse / numPoints)
       << endl;
}

// save error analysis table to file
void LeastSquareParabola::saveErrorTable(string filename) const {
  if (!fitted) fit();

  ofstream fout(filename);
  if (!fout)
    throw MatrixException("can't open file '" + filename + "' for writing");

  fout << fixed << setprecision(6);
  fout << "# Error Analysis - Least Squares Parabola Fit" << endl;
  fout << "# Equation: y = " << coeffA;
  if (coeffB >= 0)
    fout << " + " << coeffB << "x";
  else
    fout << " - " << fabs(coeffB) << "x";
  if (coeffC >= 0)
    fout << " + " << coeffC << "x^2";
  else
    fout << " - " << fabs(coeffC) << "x^2";
  fout << endl;
  fout << "# x  y_actual  y_predicted  error  error_squared" << endl;

  double sse = 0.0;
  for (int i = 0; i < numPoints; i++) {
    double xi = getX(i);
    double yActual = getY(i);
    double yPred = evaluate(xi);
    double err = yActual - yPred;
    double err2 = err * err;
    sse += err2;

    fout << xi << "  " << yActual << "  " << yPred << "  "
         << err << "  " << err2 << endl;
  }

  fout << endl;
  fout << "# Total SSE: " << sse << endl;
  fout << "# MSE: " << sse / numPoints << endl;
  fout << "# RMSE: " << sqrt(sse / numPoints) << endl;
  fout.close();

  cout << "Error analysis saved to " << filename << endl;
}

// coefficient getters
double LeastSquareParabola::getA() const {
  if (!fitted) fit();
  return coeffA;
}

double LeastSquareParabola::getB() const {
  if (!fitted) fit();
  return coeffB;
}

double LeastSquareParabola::getC() const {
  if (!fitted) fit();
  return coeffC;
}
