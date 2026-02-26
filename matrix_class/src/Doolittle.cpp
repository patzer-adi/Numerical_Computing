#include "../include/LUDecomposition.hpp"
#include <iostream>
using namespace std;

// Doolittle constructors
Doolittle::Doolittle() : LUDecomposition() {}
Doolittle::Doolittle(int r, int c) : LUDecomposition(r, c) {}

// TODO: implement Doolittle LU decomposition here
// Doolittle: L has 1s on diagonal, U is upper triangular
// Decompose A into L*U, then solve Ly = b (forward sub), then Ux = y (back sub)
double *Doolittle::solve(double *b, int n) {
  throw MatrixException(
      "Doolittle LU not implemented yet... its on the to-do list 📝");
  return nullptr;
}
