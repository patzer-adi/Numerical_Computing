#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

#include "Matrix.hpp"
#include <string>
using namespace std;

// Interpolation — abstract base class for all interpolation methods
// uses Matrix objects to store x and y data points (composition)

class Interpolation {
protected:
  Matrix xData;    // 1×n row vector of x coordinates
  Matrix yData;    // 1×n row vector of y coordinates
  int numPoints;   // number of data points

public:
  // constructors
  Interpolation();
  Interpolation(const Matrix &x, const Matrix &y);
  virtual ~Interpolation() = default;

  // load data points from Matrix objects
  void loadData(const Matrix &x, const Matrix &y);

  // getters
  int getNumPoints() const;
  double getX(int i) const;
  double getY(int i) const;
  bool hasData() const;

  // pure virtual — evaluate the interpolation/fitted curve at a single x
  // each derived class (Lagrange, LeastSquareLine, etc.) implements this
  virtual double evaluate(double x) const = 0;

  // print fitting info (equation, coefficients) — no-op by default
  // overridden by curve fitting classes (LeastSquareLine, LeastSquareParabola)
  virtual void printFitInfo();

  // error analysis — no-op by default, overridden by curve fitting classes
  virtual bool hasErrorAnalysis() const;
  virtual void printErrorTable() const;
  virtual void saveErrorTable(string filename) const;

  // interpolate over the full range [xMin, xMax] with given sample count
  void interpolate(int samples, bool saveToFile, string filename);

  void interpolate(double queryX, bool saveToFile, string filename);
};

#endif
