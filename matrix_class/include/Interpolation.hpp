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
  virtual ~Interpolation();

  // load data points from Matrix objects
  void loadData(const Matrix &x, const Matrix &y);

  // getters
  int getNumPoints() const;
  double getX(int i) const;
  double getY(int i) const;

  // pure virtual — evaluate the interpolation polynomial at a single x
  // each derived class (Lagrange, etc.) implements this
  virtual double evaluate(double x) = 0;

  // interpolate over the full range [xMin, xMax] with given sample count
  void interpolate(int samples, bool saveToFile, string filename);

  void interpolate(double queryX, bool saveToFile, string filename);
};

#endif
