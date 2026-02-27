#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "../include/GaussianElimination.hpp"
#include "../include/LUDecomposition.hpp"
#include <string>

using namespace std;

// display a matrix nicely
void displayMatrix(double **data, int rows, int cols);

// display a solution vector
void displaySolution(double *x, int n);

// write solution to a file
void writeSolutionToFile(double *x, int n, string filename);

// ask user if they want to save, and save if yes
void askToSaveSolution(double *x, int n);

// === Complete solve workflows (handles everything internally) ===

// solve using Gaussian elimination — handles input, solve, display, save
void solveGaussian(GaussianElimination &ge, bool withPivoting);

// solve using LU decomposition — handles input, solve, display, save
void solveLU(LUDecomposition &solver);

#endif
