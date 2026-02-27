#ifndef INPUT_HPP
#define INPUT_HPP

#include "../include/Matrix.hpp"
#include <string>

using namespace std;

// === Low-level read functions ===

// read matrix from console - user types row by row
void readMatrixFromConsole(double **data, int rows, int cols);

// read matrix from a file (space separated values)
// first line can have "rows cols" or just data directly
void readMatrixFromFile(string filename, double **&data, int &rows, int &cols);

// read RHS vector from file (one value per line)
void readRHSFromFile(string filename, double *&b, int &n);

// read augmented matrix and split into A and b
void readAugmentedFromFile(string filename, double **&A, double *&b, int &n);

// figure out if the file is left(matrix) or right(vector) based on name
char guessFileType(string filename);

// === High-level class-based functions (used by GE, LU, main) ===

// asks user "manual or file?" and populates a Matrix object
void getMatrixInput(Matrix &mat);

// asks user how to input Ax=b, populates Matrix A and vector b
// allocates b internally — caller must delete[] b when done
void getSystemInput(Matrix &A, double *&b, int &n);

// load A from matrix file, b from rhs file, into Matrix object
void loadSystemFromFiles(Matrix &A, double *&b, int &n, string matrixFile,
                         string rhsFile);

// load augmented [A|b] from one file into Matrix object
void loadSystemAugmented(Matrix &A, double *&b, int &n, string filename);

#endif
