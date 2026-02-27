#include "Display.hpp"
#include "Input.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

void displayMatrix(double **data, int rows, int cols) {
  cout << fixed << setprecision(6);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++)
      cout << setw(14) << data[i][j];
    cout << endl;
  }
  cout << endl;
}

void displaySolution(double *x, int n) {
  cout << "\n--- Solution ---\n";
  cout << fixed << setprecision(6);
  for (int i = 0; i < n; i++)
    cout << "x[" << i << "] = " << x[i] << endl;
  cout << endl;
}

void writeSolutionToFile(double *x, int n, string filename) {
  ofstream fout(filename);
  if (!fout) {
    cout << "could not open file for writing... oh well\n";
    return;
  }
  fout << fixed << setprecision(6);
  for (int i = 0; i < n; i++)
    fout << x[i] << endl;
  fout.close();
  cout << "Solution written to " << filename << endl;
}

void askToSaveSolution(double *x, int n) {
  cout << "Save solution to file? (1=yes, 0=no): ";
  int save;
  cin >> save;
  if (save == 1) {
    string outFile;
    cout << "Enter output filename: ";
    cin >> outFile;
    writeSolutionToFile(x, n, outFile);
  }
}

// === Complete solve workflows ===

// handles: input -> solve -> display -> save -> cleanup
void solveGaussian(GaussianElimination &ge, bool withPivoting) {
  double *b = nullptr;
  int n;
  getSystemInput(ge, b, n);

  double *x;
  if (withPivoting)
    x = ge.solveWithPivoting(b, n);
  else
    x = ge.solveWithoutPivoting(b, n);

  displaySolution(x, n);
  askToSaveSolution(x, n);

  delete[] x;
  delete[] b;
}

// handles: input -> solve -> display -> save -> cleanup
void solveLU(LUDecomposition &solver) {
  double *b = nullptr;
  int n;
  getSystemInput(solver, b, n);

  double *x = solver.solve(b, n);

  displaySolution(x, n);
  askToSaveSolution(x, n);

  delete[] x;
  delete[] b;
}
