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
  cout << "\n--- Solution ---" << endl;
  cout << fixed << setprecision(6);
  for (int i = 0; i < n; i++)
    cout << "x[" << i << "] = " << x[i] << endl;
  cout << endl;
}

void writeSolutionToFile(double *x, int n, string filename) {
  ofstream fout(filename);
  if (!fout) {
    cout << "could not open file for writing... oh well" << endl;
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

  SolverResult res = solver.solve(b, n);

  // print LU verification info (since core no longer prints)
  if (res.error < 1e-6)
    cout << "LU verification PASSED (max error: " << res.error << ")" << endl;
  else
    cout << "LU verification FAILED (max error: " << res.error << ")" << endl;

  displaySolution(res.x, res.n);
  askToSaveSolution(res.x, res.n);

  delete[] res.x;
  delete[] b;
}

// handles: input -> solve -> display -> save -> cleanup
// prints solver status (converged/diverged, iterations, etc.)
// uses polymorphism — works for Jacobi, Seidel, or any future iterative solver
void solveIterative(SystemOfLinearEquationSolver &solver, const string &methodName) {
  double *b = nullptr;
  int n;
  getSystemInput(solver, b, n);

  // print diagonal dominance status (the core no longer does this)
  if (!solver.isDiagonallyDominant()) {
    cout << "matrix is NOT diagonally dominant... trying to fix it by swapping rows" << endl;
  }

  SolverResult res = solver.solve(b, n);

  // print status (the UI layer's job)
  if (res.converged) {
    cout << methodName << " converged in " << res.iterations << " iterations" << endl;
  } else {
    cout << methodName << " DIVERGED at iteration " << res.iterations
         << " (got NaN/Inf)... system might not be suitable for this method" << endl;
  }

  displaySolution(res.x, res.n);
  askToSaveSolution(res.x, res.n);

  delete[] res.x;
  delete[] b;
}
