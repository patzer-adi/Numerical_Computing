#include "../include/LUDecomposition.hpp"
#include <iostream>
using namespace std;

// Crout constructors
Crout::Crout() : LUDecomposition() {}
Crout::Crout(int r, int c) : LUDecomposition(r, c) {}

// TODO: implement Crout LU decomposition here
// Crout: U has 1s on diagonal, L is lower triangular
// Decompose A into L*U, then solve Ly = b (forward sub), then Ux = y (back sub)
double *Crout::solve(double *b, int n) {
  throw MatrixException(
      "Crout LU not implemented yet... patience is a virtue ");
  return nullptr;
}
