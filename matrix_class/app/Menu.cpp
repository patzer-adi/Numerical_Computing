#include "Menu.hpp"
#include "../include/ForwardDifference.hpp"
#include "../include/BackwardDifference.hpp"
#include "../include/CentralDifference.hpp"
#include "../include/RichardsonExtrapolation.hpp"
#include "../include/GershgorinAnalyzer.hpp"
#include "../include/GaussJacobi.hpp"
#include "../include/GaussSeidel.hpp"
#include "../include/GaussianElimination.hpp"
#include "../include/Lagrange.hpp"
#include "../include/LeastSquareLine.hpp"
#include "../include/LeastSquareParabola.hpp"
#include "../include/LUDecomposition.hpp"
#include "../include/Matrix.hpp"
#include "../utils/Display.hpp"
#include "../utils/Input.hpp"
#include <cmath>
#include <iostream>

#ifdef USE_CUDA
#include "../cuda/include/gpu_backend.cuh"
#include "../cuda/include/gpu_dispatch.cuh"
#endif

using namespace std;

// === Handler functions — one per menu action ===

static void handleAdd() {
  cout << "\n--- Matrix A ---" << endl;
  Matrix A;
  getMatrixInput(A);

  cout << "\n--- Matrix B ---" << endl;
  Matrix B;
  getMatrixInput(B);

  Matrix C = A + B;
  cout << "\nResult (A + B):" << endl;
  cout << C;
}

static void handleSubtract() {
  cout << "\n--- Matrix A ---" << endl;
  Matrix A;
  getMatrixInput(A);

  cout << "\n--- Matrix B ---" << endl;
  Matrix B;
  getMatrixInput(B);

  Matrix C = A - B;
  cout << "\nResult (A - B):" << endl;
  cout << C;
}

static void handleMultiply() {
  cout << "\n--- Matrix A ---" << endl;
  Matrix A;
  getMatrixInput(A);

  cout << "\n--- Matrix B ---" << endl;
  Matrix B;
  getMatrixInput(B);

  Matrix C = A * B;
  cout << "\nResult (A * B):" << endl;
  cout << C;
}

static void handleDeterminant() {
  cout << "\n--- Enter square matrix ---" << endl;
  Matrix A;
  getMatrixInput(A);
  cout << "\nDeterminant = " << A.determinant() << endl;
}

static void handleGaussianWithPivoting() {
  GaussianElimination ge;
  solveGaussian(ge, true);
}

static void handleGaussianWithoutPivoting() {
  GaussianElimination ge;
  solveGaussian(ge, false);
}

static void handleDoolittle() {
  Doolittle d;
  solveLU(d);
}

static void handleCrout() {
  Crout c;
  solveLU(c);
}

static void handleCholesky() {
  Cholesky ch;
  solveLU(ch);
}

static void handleJacobi() {
  GaussJacobi gj;
  solveIterative(gj, "Gauss-Jacobi");
}

static void handleSeidel() {
  GaussSeidel gs;
  solveIterative(gs, "Gauss-Seidel");
}

static void handleTranspose() {
  cout << "\n--- Enter matrix to transpose ---" << endl;
  Matrix A;
  getMatrixInput(A);
  Matrix T = A.transpose();
  cout << "\nTranspose:" << endl;
  cout << T;
}

static void handleScalarMultiply() {
  cout << "\n--- Enter matrix ---" << endl;
  Matrix A;
  getMatrixInput(A);
  double scalar;
  cout << "Enter scalar value: ";
  cin >> scalar;
  Matrix S = A * scalar;
  cout << "\nResult (A * " << scalar << "):" << endl;
  cout << S;
}

static void handleInverse() {
  cout << "\n--- Enter square matrix ---" << endl;
  Matrix A;
  getMatrixInput(A);
  Matrix inv = A.inverse();
  cout << "\nInverse:" << endl;
  cout << inv;
}

static void handleMinor() {
  cout << "\n--- Enter square matrix ---" << endl;
  Matrix A;
  getMatrixInput(A);
  int r, c;
  cout << "Enter row to remove (0-indexed): ";
  cin >> r;
  cout << "Enter col to remove (0-indexed): ";
  cin >> c;
  Matrix M = A.minorMatrix(r, c);
  cout << "\nMinor Matrix (removed row " << r << ", col " << c << "):" << endl;
  cout << M;
}

static void handleCofactor() {
  cout << "\n--- Enter square matrix ---" << endl;
  Matrix A;
  getMatrixInput(A);
  int r, c;
  cout << "Enter row (0-indexed): ";
  cin >> r;
  cout << "Enter col (0-indexed): ";
  cin >> c;
  double cof = A.cofactor(r, c);
  cout << "\nCofactor(" << r << ", " << c << ") = " << cof << endl;
}

static void handleAdjoint() {
  cout << "\n--- Enter square matrix ---" << endl;
  Matrix A;
  getMatrixInput(A);
  Matrix adj = A.adjoint();
  cout << "\nAdjoint:" << endl;
  cout << adj;
}

static void handleCheckSquare() {
  cout << "\n--- Enter matrix ---" << endl;
  Matrix A;
  getMatrixInput(A);
  if (A.isSquare())
    cout << "\nyes, the matrix is square (" << A.getRows() << "x"
         << A.getCols() << ")" << endl;
  else
    cout << "\nnope, the matrix is not square (" << A.getRows() << "x"
         << A.getCols() << ")" << endl;
}

static void handleCheckSymmetric() {
  cout << "\n--- Enter matrix ---" << endl;
  Matrix A;
  getMatrixInput(A);
  if (A.isSymmetric())
    cout << "\nyes, the matrix is symmetric" << endl;
  else
    cout << "\nnope, the matrix is not symmetric" << endl;
}

static void handleCheckIdentity() {
  cout << "\n--- Enter matrix ---" << endl;
  Matrix A;
  getMatrixInput(A);
  if (A.isIdentity())
    cout << "\nyes, the matrix is an identity matrix" << endl;
  else
    cout << "\nnope, the matrix is not an identity matrix" << endl;
}

static void handleCheckNull() {
  cout << "\n--- Enter matrix ---" << endl;
  Matrix A;
  getMatrixInput(A);
  if (A.isNull())
    cout << "\nyes, the matrix is a null (zero) matrix" << endl;
  else
    cout << "\nnope, the matrix is not a null matrix" << endl;
}

static void handleCheckDiagonal() {
  cout << "\n--- Enter matrix ---" << endl;
  Matrix A;
  getMatrixInput(A);
  if (A.isDiagonal())
    cout << "\nyes, the matrix is diagonal" << endl;
  else
    cout << "\nnope, the matrix is not diagonal" << endl;
}

static void handleCheckDiagDominant() {
  cout << "\n--- Enter matrix ---" << endl;
  Matrix A;
  getMatrixInput(A);
  if (A.isDiagonallyDominant())
    cout << "\nyes, the matrix is diagonally dominant" << endl;
  else
    cout << "\nnope, the matrix is not diagonally dominant" << endl;
}

static void handleMakeDiagDominant() {
  cout << "\n--- Enter matrix ---" << endl;
  Matrix A;
  getMatrixInput(A);
  Matrix D = A.makeDiagonallyDominant();
  if (D.isDiagonallyDominant())
    cout << "\nsuccessfully made the matrix diagonally dominant!" << endl;
  else
    cout << "\ntried my best but this matrix can't be made diagonally dominant "
            "just by swapping rows... sorry bro" << endl;
  cout << "\nResult:" << endl;
  cout << D;
}

static void handleEquality() {
  cout << "\n--- Matrix A ---" << endl;
  Matrix A;
  getMatrixInput(A);
  cout << "\n--- Matrix B ---" << endl;
  Matrix B;
  getMatrixInput(B);
  if (A == B)
    cout << "\nyes, the matrices are equal" << endl;
  else
    cout << "\nnope, the matrices are not equal" << endl;
}

static void handleGershgorin() {
  cout << "\n--- Enter square matrix for eigenvalue analysis ---" << endl;
  Matrix A;
  getMatrixInput(A);
  GershgorinAnalyzer ga(A);
  ga.solve();
  ga.printAnalysis();
}

static void handleLagrange() {
  Lagrange lag;
  solveInterpolation(lag, "Lagrange Interpolation");
}

static void handleLeastSquareLine() {
  LeastSquareLine lsl;
  solveInterpolation(lsl, "Least Squares Line Fit");
}

static void handleLeastSquareParabola() {
  LeastSquareParabola lsp;
  solveInterpolation(lsp, "Least Squares Parabola Fit");
}

// --- helper functions for differentiation ---
static double f_exp(double x) { return exp(x); }
static double df_exp(double x) { return exp(x); }

static double f_sin(double x) { return sin(x); }
static double df_sin(double x) { return cos(x); }

static double f_cos(double x) { return cos(x); }
static double df_cos(double x) { return -sin(x); }

static double f_poly(double x) { return x*x*x - 2*x + 1; }
static double df_poly(double x) { return 3*x*x - 2; }

static void handleDifferentiation() {
  // get x0 and step sizes from user
  double x0;
  cout << "Enter the point x0 to evaluate derivatives at: ";
  cin >> x0;

  int numH;
  cout << "How many step sizes (h values)? ";
  cin >> numH;

  double *h = new double[numH];
  cout << "Enter " << numH << " step sizes: ";
  for (int i = 0; i < numH; i++)
    cin >> h[i];

  // create all four method objects
  ForwardDifference fwd;
  BackwardDifference bwd;
  CentralDifference cen;
  RichardsonExtrapolation rich;

  // register functions in all four (polymorphism via base pointer)
  Differentiation *methods[] = {&fwd, &bwd, &cen, &rich};
  int numMethods = 4;

  for (int m = 0; m < numMethods; m++) {
    methods[m]->addFunction("e^x", f_exp, df_exp);
    methods[m]->addFunction("sin(x)", f_sin, df_sin);
    methods[m]->addFunction("cos(x)", f_cos, df_cos);
    methods[m]->addFunction("x^3-2x+1", f_poly, df_poly);
    methods[m]->setStepSizes(h, numH);
    methods[m]->computeAll(x0);
  }

  // display all results
  cout << "\n--- Differentiation Results ---" << endl;
  for (int m = 0; m < numMethods; m++) {
    methods[m]->display();
  }

  int saveChoice;
  cout << "Save results to file? (1=yes, 0=no): ";
  cin >> saveChoice;
  if (saveChoice == 1) {
    string filename;
    cout << "Enter base filename (e.g. 'diff'): ";
    cin >> filename;
    for (int m = 0; m < numMethods; m++) {
      string fname = filename + "_" + to_string(m + 1) + ".txt";
      methods[m]->saveResults(fname);
    }
  }

  delete[] h;
}

// === Main menu loop ===

void runMenu() {
  cout << "\n=== Matrix Operations Program ===" << endl;

#ifdef USE_CUDA
  BackendDispatcher::printBackendInfo();
#else
  cout << "=== CPU Backend Active ===" << endl;
#endif

  cout << endl;

  int choice;
  bool running = true;

  while (running) {
    cout << "\n=== Menu ===" << endl;
    cout << "1.  Add (A + B)" << endl;
    cout << "2.  Subtract (A - B)" << endl;
    cout << "3.  Multiply (A * B)" << endl;
    cout << "4.  Determinant" << endl;
    cout << "5.  Gaussian elimination (with pivoting)" << endl;
    cout << "6.  Gaussian elimination (without pivoting)" << endl;
    cout << "7.  LU - Doolittle" << endl;
    cout << "8.  LU - Crout" << endl;
    cout << "9.  LU - Cholesky" << endl;
    cout << "10. Gauss-Jacobi (iterative)" << endl;
    cout << "11. Gauss-Seidel (iterative)" << endl;
    cout << "12. Transpose" << endl;
    cout << "13. Scalar Multiply" << endl;
    cout << "14. Inverse" << endl;
    cout << "15. Minor Matrix" << endl;
    cout << "16. Cofactor" << endl;
    cout << "17. Adjoint" << endl;
    cout << "18. Check if Square" << endl;
    cout << "19. Check if Symmetric" << endl;
    cout << "20. Check if Identity" << endl;
    cout << "21. Check if Null" << endl;
    cout << "22. Check if Diagonal" << endl;
    cout << "23. Check if Diagonally Dominant" << endl;
    cout << "24. Make Diagonally Dominant" << endl;
    cout << "25. Check Equality (A == B)" << endl;
    cout << "26. Gershgorin Eigenvalue Analysis" << endl;
    cout << "27. Lagrange Interpolation" << endl;
    cout << "28. Least Squares Line Fit" << endl;
    cout << "29. Least Squares Parabola Fit" << endl;
    cout << "30. Numerical Differentiation" << endl;
    cout << "31. Exit" << endl;
    cout << "Enter choice: ";
    cin >> choice;

    try {
      switch (choice) {
      case 1:  handleAdd(); break;
      case 2:  handleSubtract(); break;
      case 3:  handleMultiply(); break;
      case 4:  handleDeterminant(); break;
      case 5:  handleGaussianWithPivoting(); break;
      case 6:  handleGaussianWithoutPivoting(); break;
      case 7:  handleDoolittle(); break;
      case 8:  handleCrout(); break;
      case 9:  handleCholesky(); break;
      case 10: handleJacobi(); break;
      case 11: handleSeidel(); break;
      case 12: handleTranspose(); break;
      case 13: handleScalarMultiply(); break;
      case 14: handleInverse(); break;
      case 15: handleMinor(); break;
      case 16: handleCofactor(); break;
      case 17: handleAdjoint(); break;
      case 18: handleCheckSquare(); break;
      case 19: handleCheckSymmetric(); break;
      case 20: handleCheckIdentity(); break;
      case 21: handleCheckNull(); break;
      case 22: handleCheckDiagonal(); break;
      case 23: handleCheckDiagDominant(); break;
      case 24: handleMakeDiagDominant(); break;
      case 25: handleEquality(); break;
      case 26: handleGershgorin(); break;
      case 27: handleLagrange(); break;
      case 28: handleLeastSquareLine(); break;
      case 29: handleLeastSquareParabola(); break;
      case 30: handleDifferentiation(); break;
      case 31:
        cout << "\nbye bye!" << endl;
        running = false;
        break;
      default:
        cout << "\ninvalid choice... try again" << endl;
        break;
      }
    } catch (MatrixException &e) {
      e.printError();
    }
  }
}
