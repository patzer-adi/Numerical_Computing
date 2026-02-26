#include "../include/LUDecomposition.hpp"
#include <cmath>
#include <iostream>
using namespace std;

// Cholesky constructors
Cholesky::Cholesky() : LUDecomposition() {}
Cholesky::Cholesky(int r, int c) : LUDecomposition(r, c) {}

// TODO: implement Cholesky decomposition here
// Cholesky: for symmetric positive definite matrices
// Decompose A into L*L^T, then solve Ly = b (forward sub), then L^T x = y (back
// sub)
double *Cholesky::solve(double *b, int n) {
  throw MatrixException(
      "Cholesky not implemented yet... SPD matrices can wait ");
  return nullptr;
}
