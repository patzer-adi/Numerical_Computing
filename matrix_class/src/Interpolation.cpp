#include "../include/Interpolation.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

// default constructor
Interpolation::Interpolation() {
  xData = nullptr;
  yData = nullptr;
  numPoints = 0;
}

// parameterized constructor — copies the input arrays
Interpolation::Interpolation(double *x, double *y, int n) {
  numPoints = n;
  xData = new double[n];
  yData = new double[n];
  for (int i = 0; i < n; i++) {
    xData[i] = x[i];
    yData[i] = y[i];
  }
}

// destructor — clean up owned arrays
Interpolation::~Interpolation() {
  if (xData != nullptr) {
    delete[] xData;
    xData = nullptr;
  }
  if (yData != nullptr) {
    delete[] yData;
    yData = nullptr;
  }
}

// load data points — replaces any existing data
void Interpolation::loadData(double *x, double *y, int n) {
  // free old data
  if (xData != nullptr) delete[] xData;
  if (yData != nullptr) delete[] yData;

  numPoints = n;
  xData = new double[n];
  yData = new double[n];
  for (int i = 0; i < n; i++) {
    xData[i] = x[i];
    yData[i] = y[i];
  }
}

// getters
int Interpolation::getNumPoints() const { return numPoints; }

double Interpolation::getX(int i) const {
  if (i < 0 || i >= numPoints)
    throw MatrixException("index out of bounds in getX()");
  return xData[i];
}

double Interpolation::getY(int i) const {
  if (i < 0 || i >= numPoints)
    throw MatrixException("index out of bounds in getY()");
  return yData[i];
}

// interpolate over the full range [xMin, xMax]
// generates evenly spaced sample points and evaluates the polynomial
// output is gnuplot-compatible: space-separated x y pairs
void Interpolation::interpolate(int samples, bool saveToFile, string filename) {
  if (numPoints < 2)
    throw MatrixException("need at least 2 data points to interpolate");

  double xMin = xData[0];
  double xMax = xData[numPoints - 1];
  double step = (xMax - xMin) / (samples - 1);

  if (saveToFile) {
    ofstream fout(filename);
    if (!fout)
      throw MatrixException("can't open file '" + filename + "' for writing");

    fout << fixed << setprecision(6);

    // header comments for gnuplot
    fout << "# Interpolation Result" << endl;
    fout << "# Data points: " << numPoints << endl;
    fout << "# Sample points: " << samples << endl;
    fout << "# Range: [" << xMin << ", " << xMax << "]" << endl;
    fout << "# x  y" << endl;

    // interpolated curve
    for (int i = 0; i < samples; i++) {
      double x = xMin + i * step;
      double y = evaluate(x);
      fout << x << "  " << y << endl;
    }

    // blank lines to separate datasets (gnuplot index)
    fout << endl << endl;

    // original data points
    fout << "# Original data points" << endl;
    for (int i = 0; i < numPoints; i++) {
      fout << xData[i] << "  " << yData[i] << endl;
    }

    fout.close();
    cout << "Interpolation data written to " << filename << endl;

  } else {
    // print to console
    cout << fixed << setprecision(6);
    cout << "\n--- Interpolated Values ---" << endl;
    cout << left << setw(16) << "x" << setw(16) << "y" << endl;
    cout << string(32, '-') << endl;

    for (int i = 0; i < samples; i++) {
      double x = xMin + i * step;
      double y = evaluate(x);
      cout << left << setw(16) << x << setw(16) << y << endl;
    }

    cout << "\n--- Original Data Points ---" << endl;
    for (int i = 0; i < numPoints; i++) {
      cout << left << setw(16) << xData[i] << setw(16) << yData[i] << endl;
    }
    cout << endl;
  }
}

// interpolate at a specific query point
void Interpolation::interpolate(double queryX, bool saveToFile, string filename) {
  if (numPoints < 2)
    throw MatrixException("need at least 2 data points to interpolate");

  double result = evaluate(queryX);

  if (saveToFile) {
    ofstream fout(filename);
    if (!fout)
      throw MatrixException("can't open file '" + filename + "' for writing");

    fout << fixed << setprecision(6);
    fout << "# Interpolation Query Result" << endl;
    fout << "# Query point: x = " << queryX << endl;
    fout << "# Result: y = " << result << endl;
    fout << "# x  y" << endl;
    fout << queryX << "  " << result << endl;
    fout.close();
    cout << "Query result written to " << filename << endl;

  } else {
    cout << fixed << setprecision(6);
    cout << "\n--- Interpolation Query ---" << endl;
    cout << "x = " << queryX << endl;
    cout << "y = " << result << endl;
    cout << endl;
  }
}
