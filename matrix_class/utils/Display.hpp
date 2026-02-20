#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "../include/Matrix.hpp"

// Print a matrix with a label
void showMatrix(const string& label, Matrix& m);

// Print the solution from Gaussian elimination
void showSolution(double* solution, int n);

#endif