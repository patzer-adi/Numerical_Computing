#ifndef EIGENSOLVER_HPP
#define EIGENSOLVER_HPP

#include "Matrix.hpp"
#include <vector>
using namespace std;

// represents one Gershgorin disc for a single row
struct GershgorinDisc {
  int row;       // which row this disc comes from (0-indexed)
  double center; // diagonal element a_ii
  double radius; // sum of |a_ij| for j != i
  double low;    // center - radius
  double high;   // center + radius
};

// EigenSolver — eigenvalue analysis tools
// currently supports: Gershgorin Circle Theorem
//
// usage:
//   Matrix A;
//   // ... fill A ...
//   EigenSolver eigen(A);
//   eigen.printGershgorinAnalysis();
//
class EigenSolver {
private:
  const Matrix &mat;

public:
  EigenSolver(const Matrix &m);

  // compute Gershgorin discs — one per row
  vector<GershgorinDisc> computeGershgorinDiscs() const;

  // pretty-print full Gershgorin analysis
  // shows: each disc, union interval, diagonal dominance insight
  void printGershgorinAnalysis() const;
};

#endif
