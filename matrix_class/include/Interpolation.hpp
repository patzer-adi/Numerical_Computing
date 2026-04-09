#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

#include "Matrix.hpp"
#include <string>
using namespace std;

// Interpolation — abstract base class for all interpolation methods

class Interpolation {
protected:
  double *xData;   // x coordinates of data points
  double *yData;   // y coordinates of data points
  int numPoints;   // number of data points

public:
  // constructors
  Interpolation();
  Interpolation(double *x, double *y, int n);
  virtual ~Interpolation();

  // load data points (copies the arrays internally)
  void loadData(double *x, double *y, int n);

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
