#include "../include/EigenSolver.hpp"
using namespace std;

// constructor — validates square matrix
EigenSolver::EigenSolver(const Matrix &m) : mat(m) {
  if (!m.isSquare())
    throw MatrixException(
        "eigenvalue analysis needs a square matrix... "
        "eigenvalues only make sense for square matrices bro");
}

// destructor — nothing to free in base class
EigenSolver::~EigenSolver() {}
