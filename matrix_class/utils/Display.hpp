#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <string>

using namespace std;

// display a matrix nicely
void displayMatrix(double **data, int rows, int cols);

// display a solution vector
void displaySolution(double *x, int n);

// write solution to a file
void writeSolutionToFile(double *x, int n, string filename);

#endif
