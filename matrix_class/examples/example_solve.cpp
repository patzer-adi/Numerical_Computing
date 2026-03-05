// example_solve.cpp — solve a system Ax = b using all three LU methods
// Shows: Doolittle, Crout, and GaussianElimination on the same system
//
// COMPILE:
//   g++ -std=c++11 -o example_solve examples/example_solve.cpp \
//       src/Matrix.cpp src/MatrixException.cpp src/MatrixOperations.cpp \
//       src/SystemOfLinearEquationSolver.cpp src/GaussianElimination.cpp \
//       src/LUDecomposition.cpp src/Doolittle.cpp src/Crout.cpp
//
// RUN:
//   ./example_solve

#include "../include/GaussianElimination.hpp"
#include "../include/LUDecomposition.hpp"
#include <iostream>
using namespace std;

// helper: set up a 3x3 system
void setupSystem(Matrix &solver) {
  // system:  2x + y - z = 8
  //         -3x - y + 2z = -11
  //         -2x + y + 2z = -3
  Matrix temp(3, 3);
  temp.setData(0, 0, 2);
  temp.setData(0, 1, 1);
  temp.setData(0, 2, -1);
  temp.setData(1, 0, -3);
  temp.setData(1, 1, -1);
  temp.setData(1, 2, 2);
  temp.setData(2, 0, -2);
  temp.setData(2, 1, 1);
  temp.setData(2, 2, 2);
  solver.copyFrom(temp);
}

void printSolution(double *x, int n) {
  for (int i = 0; i < n; i++)
    cout << "  x[" << i << "] = " << x[i] << endl;
}

int main() {
  double b[] = {8, -11, -3};
  double *x;

  // --- Gaussian Elimination ---
  cout << "=== Gaussian Elimination ===" << endl;
  GaussianElimination ge;
  setupSystem(ge);
  x = ge.solve(b, 3);
  printSolution(x, 3);
  delete[] x;

  // --- Doolittle LU ---
  cout << "\n=== Doolittle LU ===" << endl;
  Doolittle doo;
  setupSystem(doo);
  x = doo.solve(b, 3);
  printSolution(x, 3);
  delete[] x;

  // --- Crout LU ---
  cout << "\n=== Crout LU ===" << endl;
  Crout cr;
  setupSystem(cr);
  x = cr.solve(b, 3);
  printSolution(x, 3);
  delete[] x;

  cout << "\nExpected: x = [2, 3, -1]" << endl;
  return 0;
}
