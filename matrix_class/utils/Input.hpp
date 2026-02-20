#ifndef INPUT_HPP
#define INPUT_HPP

#include "../include/Matrix.hpp"

// Fill a matrix with user input
void takeMatrixInput(Matrix& m);

// Fill the augmented matrix for Gaussian elimination
void takeGaussInput(double** mat, int n);

#endif