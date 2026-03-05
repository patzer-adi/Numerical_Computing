#include "../include/LUDecomposition.hpp"

LUDecomposition::LUDecomposition() : SystemOfLinearEquationSolver() {}
LUDecomposition::LUDecomposition(int r, int c)
    : SystemOfLinearEquationSolver(r, c) {}
