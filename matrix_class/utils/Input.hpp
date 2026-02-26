#ifndef INPUT_HPP
#define INPUT_HPP

#include <string>

using namespace std;

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
// returns 'l' for left, 'r' for right, 'a' for augmented
char guessFileType(string filename);

// get matrix input (asks user: console or file)
void getMatrixInput(double **&data, int &rows, int &cols);

// get system input (matrix A and vector b) for solving
void getSystemInput(double **&A, double *&b, int &n);

#endif
