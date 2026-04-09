#ifndef EIGENSOLVER_HPP
#define EIGENSOLVER_HPP

#include "Matrix.hpp"
using namespace std;

// GershgorinDisc — represents one Gershgorin disc for a single row
// kept here since it's a shared data structure for eigenvalue analysis
struct GershgorinDisc {
  int row;       // which row this disc comes from (0-indexed)
  double center; // diagonal element a_ii
  double radius; // sum of |a_ij| for j != i
  double low;    // center - radius
  double high;   // center + radius
};

// EigenSolver — abstract base class for eigenvalue analysis methods
//
// DESIGN: uses COMPOSITION (holds a Matrix reference), does NOT inherit Matrix.
// Same philosophy as Interpolation — eigenvalue analysis USES a matrix,
// it is NOT a matrix.
//
// Each derived class implements a different eigenvalue method:
//   - GershgorinAnalyzer  (Gershgorin Circle Theorem)
//   - PowerMethod         (future)
//   - QRMethod            (future)
//
class EigenSolver {
protected:
  const Matrix &mat;

public:
  EigenSolver(const Matrix &m);
  virtual ~EigenSolver();

  // every eigenvalue method must implement this
  virtual void solve() = 0;

  // every method should be able to print its results
  virtual void printAnalysis() const = 0;
};

#endif
