#include "../include/LeastSquareLine.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

// default constructor
LeastSquareLine::LeastSquareLine()
    : Interpolation(), coeffA(0.0), coeffB(0.0), fitted(false) {}

// parameterized constructor — takes Matrix objects
LeastSquareLine::LeastSquareLine(const Matrix &x, const Matrix &y)
    : Interpolation(x, y), coeffA(0.0), coeffB(0.0), fitted(false) {}


void LeastSquareLine::fit() const {
  if (fitted) return;

  if (numPoints < 2)
    throw MatrixException(
        "least squares line needs at least 2 data points... "
        "can't fit a line with less");

  // step 1: compute required sums from data (stored in Matrix xData, yData)
  double sumX = 0.0, sumY = 0.0, sumX2 = 0.0, sumXY = 0.0;
  int n = numPoints;

  for (int i = 0; i < n; i++) {
    double xi = getX(i);
    double yi = getY(i);
    sumX += xi;
    sumY += yi;
    sumX2 += xi * xi;
    sumXY += xi * yi;
  }

  // step 2: build the 2×2 coefficient matrix using Matrix class
  Matrix A(2, 2);
  A.setData(0, 0, (double)n);
  A.setData(0, 1, sumX);
  A.setData(1, 0, sumX);
  A.setData(1, 1, sumX2);

  // step 3: build the 2×1 RHS vector using Matrix class
  Matrix rhs(2, 1);
  rhs.setData(0, 0, sumY);
  rhs.setData(1, 0, sumXY);

  // step 4: solve using Matrix inverse and multiplication
  // coefficients = A^(-1) * rhs
  Matrix Ainv = A.inverse();
  Matrix coeffs = Ainv * rhs;

  // step 5: extract coefficients from result Matrix
  coeffA = coeffs.getData(0, 0);
  coeffB = coeffs.getData(1, 0);
  fitted = true;
}

// evaluate the fitted line at a single x
double LeastSquareLine::evaluate(double x) const {
  if (!fitted) fit();
  return coeffA + coeffB * x;
}

// print the fitted equation and coefficients
void LeastSquareLine::printFitInfo() {
  if (!fitted) fit();

  cout << fixed << setprecision(6);
  cout << "\n--- Least Squares Line Fit ---" << endl;
  cout << "Best fit line: y = a + bx" << endl;
  cout << "  a (intercept) = " << coeffA << endl;
  cout << "  b (slope)     = " << coeffB << endl;
  cout << "\n  y = " << coeffA;
  if (coeffB >= 0)
    cout << " + " << coeffB << "x";
  else
    cout << " - " << fabs(coeffB) << "x";
  cout << endl;
}

// error analysis support
bool LeastSquareLine::hasErrorAnalysis() const { return true; }

// print error analysis table — computed on-the-fly from coefficients
void LeastSquareLine::printErrorTable() const {
  if (!fitted) fit();

  cout << fixed << setprecision(6);
  cout << "\n--- Error Analysis Table (Least Squares Line) ---" << endl;
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
  cout << "RMSE (Root Mean Squared Error):    " << sqrt(sse / numPoints) << endl;
}

// save error analysis table to file
void LeastSquareLine::saveErrorTable(string filename) const {
  if (!fitted) fit();

  ofstream fout(filename);
  if (!fout)
    throw MatrixException("can't open file '" + filename + "' for writing");

  fout << fixed << setprecision(6);
  fout << "# Error Analysis - Least Squares Line Fit" << endl;
  fout << "# Equation: y = " << coeffA;
  if (coeffB >= 0)
    fout << " + " << coeffB << "x";
  else
    fout << " - " << fabs(coeffB) << "x";
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
double LeastSquareLine::getA() const {
  if (!fitted) fit();
  return coeffA;
}

double LeastSquareLine::getB() const {
  if (!fitted) fit();
  return coeffB;
}
