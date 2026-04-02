#include "../include/EigenSolver.hpp"
#include <cmath>
#include <iomanip>
#include <iostream>
using namespace std;

EigenSolver::EigenSolver(const Matrix &m) : mat(m) {
  if (!m.isSquare())
    throw MatrixException(
        "Gershgorin needs a square matrix... eigenvalues only make sense for "
        "square matrices bro");
}

vector<GershgorinDisc> EigenSolver::computeGershgorinDiscs() const {
  int n = mat.getRows();
  vector<GershgorinDisc> discs;

  for (int i = 0; i < n; i++) {
    GershgorinDisc disc;
    disc.row = i;
    disc.center = mat.getData(i, i);

    double radius = 0.0;
    for (int j = 0; j < n; j++) {
      if (j != i)
        radius += fabs(mat.getData(i, j));
    }

    disc.radius = radius;
    disc.low = disc.center - radius;
    disc.high = disc.center + radius;
    discs.push_back(disc);
  }

  return discs;
}

void EigenSolver::printGershgorinAnalysis() const {
  vector<GershgorinDisc> discs = computeGershgorinDiscs();
  int n = discs.size();

  cout << "\n========================================" << endl;
  cout << "  Gershgorin Circle Theorem Analysis" << endl;
  cout << "========================================" << endl;

  cout << "\n--- How it works ---" << endl;
  cout << "Each row gives a disc: center = diagonal element, "
       << "radius = sum of |off-diagonal elements|" << endl;
  cout << "Every eigenvalue of the matrix must lie inside at least one disc."
       << endl;

  // print each disc
  cout << "\n--- Gershgorin Discs ---" << endl;
  cout << fixed << setprecision(4);
  cout << left << setw(6) << "Row" << setw(14) << "Center" << setw(14)
       << "Radius" << setw(14) << "Low" << setw(14) << "High" << endl;
  cout << string(62, '-') << endl;

  double unionLow = discs[0].low;
  double unionHigh = discs[0].high;
  bool allDominant = true;

  for (int i = 0; i < n; i++) {
    cout << left << setw(6) << discs[i].row << setw(14) << discs[i].center
         << setw(14) << discs[i].radius << setw(14) << discs[i].low
         << setw(14) << discs[i].high << endl;

    if (discs[i].low < unionLow)
      unionLow = discs[i].low;
    if (discs[i].high > unionHigh)
      unionHigh = discs[i].high;

    // check if this row satisfies strict diagonal dominance
    if (fabs(discs[i].center) <= discs[i].radius)
      allDominant = false;
  }

  // union interval
  cout << "\n--- Union Interval ---" << endl;
  cout << "All eigenvalues lie in: [" << unionLow << ", " << unionHigh << "]"
       << endl;

  // check if all eigenvalues are positive (all discs have low > 0)
  bool allPositive = true;
  bool allNegative = true;
  bool couldBeZero = false;

  for (int i = 0; i < n; i++) {
    if (discs[i].low <= 0)
      allPositive = false;
    if (discs[i].high >= 0)
      allNegative = false;
    if (discs[i].low <= 0 && discs[i].high >= 0)
      couldBeZero = true;
  }

  // insights
  cout << "\n--- Eigenvalue Insights ---" << endl;

  if (allPositive)
    cout << "  [+] All eigenvalues are POSITIVE (every disc is entirely > 0)"
         << endl;
  else if (allNegative)
    cout << "  [-] All eigenvalues are NEGATIVE (every disc is entirely < 0)"
         << endl;
  else
    cout << "  [~] Eigenvalues may be positive, negative, or mixed" << endl;

  if (couldBeZero)
    cout << "  [!] Some disc contains 0 — matrix MIGHT be singular" << endl;
  else
    cout << "  [✓] No disc contains 0 — matrix is definitely non-singular"
         << endl;

  if (allDominant)
    cout << "  [✓] Matrix is strictly diagonally dominant (|a_ii| > radius for "
            "every row)"
         << endl;
  else
    cout << "  [~] Matrix is NOT strictly diagonally dominant" << endl;

  // check if symmetric (eigenvalues are real for symmetric matrices)
  if (mat.isSymmetric())
    cout << "  [✓] Matrix is symmetric — all eigenvalues are guaranteed REAL"
         << endl;

  cout << "\n========================================\n" << endl;
}
