#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "../include/GaussianElimination.hpp"
#include "../include/LUDecomposition.hpp"
#include "../include/SolverResult.hpp"
#include "../include/SystemOfLinearEquationSolver.hpp"
#include <string>

using namespace std;

// display a matrix nicely
void displayMatrix(double **data, int rows, int cols);

// display a solution vector
void displaySolution(double *x, int n);

// write solution to a file
void writeSolutionToFile(double *x, int n, string filename);

// write a matrix to a file
void writeMatrixToFile(double **data, int n, string filename);

// ask user if they want to save, and save if yes
void askToSaveSolution(double *x, int n);

// ask user if they want to save L and U matrices to files
void askToSaveLU(SolverResult &res);

// === Complete solve workflows (handles everything internally) ===

// solve using Gaussian elimination — handles input, solve, display, save
void solveGaussian(GaussianElimination &ge, bool withPivoting);

// solve using LU decomposition — handles input, solve, display, save
void solveLU(LUDecomposition &solver);

// solve using any iterative method (polymorphic) — handles input, solve, display, save
// also prints solver status (converged/diverged, iterations, etc.)
void solveIterative(SystemOfLinearEquationSolver &solver, const string &methodName);

#endif
