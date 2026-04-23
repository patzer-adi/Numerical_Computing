#include "Display.hpp"
#include "Input.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
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

void writeMatrixToFile(double **data, int n, string filename) {
  ofstream fout(filename);
  if (!fout) {
    cout << "could not open file for writing... oh well" << endl;
    return;
  }
  fout << fixed << setprecision(6);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (j > 0) fout << " ";
      fout << setw(14) << data[i][j];
    }
    fout << endl;
  }
  fout.close();
  cout << "Matrix written to " << filename << endl;
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

void askToSaveLU(SolverResult &res) {
  if (!res.L || !res.U || res.luSize == 0) return;

  cout << "\n--- L and U matrices are available! ---" << endl;
  cout << "\nL (Lower triangular):" << endl;
  displayMatrix(res.L, res.luSize, res.luSize);
  cout << "U (Upper triangular):" << endl;
  displayMatrix(res.U, res.luSize, res.luSize);

  cout << "Save L and U matrices to files? (1=yes, 0=no): ";
  int save;
  cin >> save;
  if (save == 1) {
    string lFile, uFile;
    cout << "Enter filename for L matrix: ";
    cin >> lFile;
    writeMatrixToFile(res.L, res.luSize, lFile);
    cout << "Enter filename for U matrix: ";
    cin >> uFile;
    writeMatrixToFile(res.U, res.luSize, uFile);
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

// handles: input -> solve -> display -> save L/U -> save solution -> cleanup
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

  // show L and U and offer to save them
  askToSaveLU(res);

  displaySolution(res.x, res.n);
  askToSaveSolution(res.x, res.n);

  res.freeLU();
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

  // print dominance result
  if (!res.dominanceAchieved) {
    cout << "WARNING: diagonal dominance could NOT be achieved after row swaps" << endl;
  }

  // print status (the UI layer's job)
  if (res.converged) {
    cout << methodName << " converged in " << res.iterations << " iterations"
         << " (error: " << res.error << ")" << endl;
  } else if (res.error == numeric_limits<double>::infinity()) {
    cout << methodName << " DIVERGED at iteration " << res.iterations
         << " (got NaN/Inf)... system might not be suitable for this method" << endl;
  } else {
    cout << methodName << " did NOT converge after " << res.iterations
         << " iterations (last max diff: " << res.error << ")" << endl;
  }

  displaySolution(res.x, res.n);
  askToSaveSolution(res.x, res.n);

  delete[] res.x;
  delete[] b;
}

// === Interpolation workflow ===
// handles: data input -> fit info -> error analysis -> mode selection -> evaluate -> display/save

void solveInterpolation(Interpolation &interp, const string &methodName) {
  // step 1: get X data points using Matrix
  cout << "\n--- Enter X data points (as a matrix) ---" << endl;
  Matrix xMat;
  getMatrixInput(xMat);

  // step 2: get Y data points using Matrix
  cout << "\n--- Enter Y data points (as a matrix) ---" << endl;
  Matrix yMat;
  getMatrixInput(yMat);

  // load Matrix objects directly into the interpolation object
  interp.loadData(xMat, yMat);

  // print loaded data
  int n = interp.getNumPoints();
  cout << "\nLoaded " << n << " data points:" << endl;
  cout << fixed << setprecision(6);
  for (int i = 0; i < n; i++)
    cout << "  (" << interp.getX(i) << ", " << interp.getY(i) << ")" << endl;

  // print fit info (equation, coefficients) — no-op for Lagrange,
  // prints equation for curve fitting methods
  interp.printFitInfo();

  // error analysis for curve fitting methods (least squares line/parabola)
  if (interp.hasErrorAnalysis()) {
    int errChoice;
    cout << "\nShow error analysis table? (1=yes, 0=no): ";
    cin >> errChoice;
    if (errChoice == 1) {
      interp.printErrorTable();

      int saveErr;
      cout << "\nSave error table to file? (1=yes, 0=no): ";
      cin >> saveErr;
      if (saveErr == 1) {
        string errFile;
        cout << "Enter filename: ";
        cin >> errFile;
        interp.saveErrorTable(errFile);
      }
    }
  }

  // step 3: choose mode
  int mode;
  cout << "\n--- " << methodName << " ---" << endl;
  cout << "1. Full interpolation (evaluate over entire range)" << endl;
  cout << "2. Query a specific point" << endl;
  cout << "Enter choice: ";
  cin >> mode;

  if (mode == 1) {
    // full interpolation
    int samples;
    cout << "Enter number of sample points (e.g. 100): ";
    cin >> samples;

    // ask save or print
    int saveChoice;
    cout << "Output option:\n1. Print to console\n2. Save to file\nEnter choice: ";
    cin >> saveChoice;

    if (saveChoice == 2) {
      string filename;
      cout << "Enter output filename: ";
      cin >> filename;
      interp.interpolate(samples, true, filename);
    } else {
      interp.interpolate(samples, false, "");
    }

  } else if (mode == 2) {
    // query specific point
    double queryX;
    cout << "Enter query x value: ";
    cin >> queryX;

    // ask save or print
    int saveChoice;
    cout << "Output option:\n1. Print to console\n2. Save to file\nEnter choice: ";
    cin >> saveChoice;

    if (saveChoice == 2) {
      string filename;
      cout << "Enter output filename: ";
      cin >> filename;
      interp.interpolate(queryX, true, filename);
    } else {
      interp.interpolate(queryX, false, "");
    }

  } else {
    throw MatrixException("invalid choice... it was 1 or 2 bro");
  }
}
