#include "../include/Interpolation.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

// default constructor
Interpolation::Interpolation() {
  numPoints = 0;
}

// parameterized constructor — takes two Matrix objects (1×n row vectors)
Interpolation::Interpolation(const Matrix &x, const Matrix &y) {
  // flatten: total elements = rows * cols
  int xSize = x.getRows() * x.getCols();
  int ySize = y.getRows() * y.getCols();

  if (xSize != ySize)
    throw MatrixException("X and Y must have the same number of data points");
  if (xSize < 1)
    throw MatrixException("need at least 1 data point");

  numPoints = xSize;

  // store as 1×n row vectors using Matrix
  xData = Matrix(1, numPoints);
  yData = Matrix(1, numPoints);

  int idx = 0;
  for (int i = 0; i < x.getRows(); i++)
    for (int j = 0; j < x.getCols(); j++)
      xData.setData(0, idx++, x.getData(i, j));

  idx = 0;
  for (int i = 0; i < y.getRows(); i++)
    for (int j = 0; j < y.getCols(); j++)
      yData.setData(0, idx++, y.getData(i, j));
}

// load data points from Matrix objects — replaces any existing data
void Interpolation::loadData(const Matrix &x, const Matrix &y) {
  int xSize = x.getRows() * x.getCols();
  int ySize = y.getRows() * y.getCols();

  if (xSize != ySize)
    throw MatrixException("X and Y must have the same number of data points");
  if (xSize < 1)
    throw MatrixException("need at least 1 data point");

  numPoints = xSize;

  // store as 1×n row vectors
  xData = Matrix(1, numPoints);
  yData = Matrix(1, numPoints);

  int idx = 0;
  for (int i = 0; i < x.getRows(); i++)
    for (int j = 0; j < x.getCols(); j++)
      xData.setData(0, idx++, x.getData(i, j));

  idx = 0;
  for (int i = 0; i < y.getRows(); i++)
    for (int j = 0; j < y.getCols(); j++)
      yData.setData(0, idx++, y.getData(i, j));
}

// getters — access data through Matrix methods
int Interpolation::getNumPoints() const { return numPoints; }

double Interpolation::getX(int i) const {
  if (i < 0 || i >= numPoints)
    throw MatrixException("index out of bounds in getX()");
  return xData.getData(0, i);
}

double Interpolation::getY(int i) const {
  if (i < 0 || i >= numPoints)
    throw MatrixException("index out of bounds in getY()");
  return yData.getData(0, i);
}

bool Interpolation::hasData() const { return numPoints > 0; }

// default implementations — overridden by curve fitting classes
void Interpolation::printFitInfo() {}
bool Interpolation::hasErrorAnalysis() const { return false; }
void Interpolation::printErrorTable() const {}
void Interpolation::saveErrorTable(string filename) const {
  (void)filename; // suppress unused parameter warning
}

// interpolate over the full range [xMin, xMax]
// generates evenly spaced sample points and evaluates the polynomial
// output is gnuplot-compatible: space-separated x y pairs
void Interpolation::interpolate(int samples, bool saveToFile, string filename) {
  if (numPoints < 2)
    throw MatrixException("need at least 2 data points to interpolate");
  if (samples < 2)
    throw MatrixException("samples must be >= 2");

  // find actual min and max (don't assume sorted input)
  double xMin = getX(0);
  double xMax = xMin;
  for (int i = 1; i < numPoints; i++) {
    double xi = getX(i);
    if (xi < xMin) xMin = xi;
    if (xi > xMax) xMax = xi;
  }

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
      fout << getX(i) << "  " << getY(i) << endl;
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
      cout << left << setw(16) << getX(i)
           << setw(16) << getY(i) << endl;
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
