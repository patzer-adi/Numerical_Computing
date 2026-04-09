#ifndef GERSHGORINANALYZER_HPP
#define GERSHGORINANALYZER_HPP

#include "EigenSolver.hpp"

// GershgorinAnalyzer — Gershgorin Circle Theorem implementation
//
// Computes Gershgorin discs for each row of a square matrix.
// Each disc has center = diagonal element, radius = sum of |off-diagonal|.
// Every eigenvalue of the matrix must lie inside at least one disc.
//
// usage:
//   Matrix A;
//   // ... fill A ...
//   GershgorinAnalyzer ga(A);
//   ga.solve();
//   ga.printAnalysis();

class GershgorinAnalyzer : public EigenSolver {
private:
  GershgorinDisc *discs;  // array of discs (owned, new[])
  int numDiscs;           // number of discs (= matrix size)

public:
  GershgorinAnalyzer(const Matrix &m);
  ~GershgorinAnalyzer();

  // compute Gershgorin discs — one per row
  void solve() override;

  // pretty-print full Gershgorin analysis
  // shows: each disc, union interval, diagonal dominance insight
  void printAnalysis() const override;

  // getters
  int getNumDiscs() const;
  GershgorinDisc getDisc(int i) const;
};

#endif
