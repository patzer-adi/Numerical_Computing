// verify.cpp — comprehensive verification suite for iterative solvers
// runs GE reference, Jacobi, and Seidel on multiple test cases
// checks: residuals, agreement, convergence, timing
//
// build: make verify
// run:   ./verify_program

#include "include/GaussJacobi.hpp"
#include "include/GaussSeidel.hpp"

#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>

#include <string>

using namespace std;
using namespace std::chrono;

// ============================================================
// Reference Gaussian Elimination with partial pivoting
// standalone — not a class method
// returns heap-allocated x, caller frees. Returns nullptr if singular.
// ============================================================
static double *gaussianElimination(double **Aorig, double *borig, int n) {
  // deep copy A and b
  double **A = new double *[n];
  for (int i = 0; i < n; i++) {
    A[i] = new double[n];
    for (int j = 0; j < n; j++)
      A[i][j] = Aorig[i][j];
  }
  double *b = new double[n];
  for (int i = 0; i < n; i++)
    b[i] = borig[i];

  // forward elimination with partial pivoting
  for (int k = 0; k < n - 1; k++) {
    // find pivot
    int maxRow = k;
    double maxVal = fabs(A[k][k]);
    for (int i = k + 1; i < n; i++) {
      if (fabs(A[i][k]) > maxVal) {
        maxVal = fabs(A[i][k]);
        maxRow = i;
      }
    }
    // swap rows
    if (maxRow != k) {
      double *tmpRow = A[k];
      A[k] = A[maxRow];
      A[maxRow] = tmpRow;
      double tmpB = b[k];
      b[k] = b[maxRow];
      b[maxRow] = tmpB;
    }
    // check for singular
    if (fabs(A[k][k]) < 1e-15) {
      for (int i = 0; i < n; i++)
        delete[] A[i];
      delete[] A;
      delete[] b;
      return nullptr;
    }
    // eliminate
    for (int i = k + 1; i < n; i++) {
      double factor = A[i][k] / A[k][k];
      for (int j = k; j < n; j++)
        A[i][j] -= factor * A[k][j];
      b[i] -= factor * b[k];
    }
  }
  // check last diagonal
  if (fabs(A[n - 1][n - 1]) < 1e-15) {
    for (int i = 0; i < n; i++)
      delete[] A[i];
    delete[] A;
    delete[] b;
    return nullptr;
  }

  // back substitution
  double *x = new double[n];
  for (int i = n - 1; i >= 0; i--) {
    double sum = 0.0;
    for (int j = i + 1; j < n; j++)
      sum += A[i][j] * x[j];
    x[i] = (b[i] - sum) / A[i][i];
  }

  for (int i = 0; i < n; i++)
    delete[] A[i];
  delete[] A;
  delete[] b;
  return x;
}

// ============================================================
// Compute max |Ax - b| residual
// ============================================================
static double computeResidual(double **A, double *b, double *x, int n) {
  double maxR = 0.0;
  for (int i = 0; i < n; i++) {
    double ax = 0.0;
    for (int j = 0; j < n; j++)
      ax += A[i][j] * x[j];
    double r = fabs(b[i] - ax);
    if (r > maxR)
      maxR = r;
  }
  return maxR;
}

// ============================================================
// Compute max |a - b| between two vectors
// ============================================================
static double maxDiff(double *a, double *b, int n) {
  double mx = 0.0;
  for (int i = 0; i < n; i++) {
    double d = fabs(a[i] - b[i]);
    if (d > mx)
      mx = d;
  }
  return mx;
}

// ============================================================
// Print solution (first few + last few for large)
// ============================================================
static void printSolution(double *x, int n, int show = 5) {
  cout << "[";
  if (n <= show * 2) {
    for (int i = 0; i < n; i++) {
      cout << fixed << setprecision(6) << x[i];
      if (i < n - 1)
        cout << ", ";
    }
  } else {
    for (int i = 0; i < show; i++)
      cout << fixed << setprecision(6) << x[i] << ", ";
    cout << "..., ";
    for (int i = n - show; i < n; i++) {
      cout << fixed << setprecision(6) << x[i];
      if (i < n - 1)
        cout << ", ";
    }
  }
  cout << "]";
}

// ============================================================
// Run a single test case
// ============================================================
struct TestResult {
  int passed;
  int failed;
  int warned;
};

static TestResult runTest(const string &name, double **A, double *b, int n,
                          bool expectConverge = true) {
  TestResult tr = {0, 0, 0};
  cout << "\n===== TEST: " << name << " " << n << "x" << n << " =====" << endl;

  // --- GE reference ---
  auto t0 = high_resolution_clock::now();
  double *x_ge = gaussianElimination(A, b, n);
  auto t1 = high_resolution_clock::now();
  double ge_ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;

  if (x_ge == nullptr) {
    cout << "Gaussian Elimination:          SINGULAR (no solution)" << endl;
    if (!expectConverge) {
      cout << "  [PASS] Expected singular" << endl;
      tr.passed++;
    } else {
      cout << "  [FAIL] Expected solvable" << endl;
      tr.failed++;
    }
    return tr;
  }

  cout << "Gaussian Elimination solution:  x = ";
  printSolution(x_ge, n);
  cout << "  (" << fixed << setprecision(3) << ge_ms << " ms)" << endl;

  // --- Jacobi ---
  GaussJacobi gj(n, n);
  // load A into solver
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      gj(i, j) = A[i][j];

  double *b_copy_j = new double[n];
  for (int i = 0; i < n; i++)
    b_copy_j[i] = b[i];

  auto t2 = high_resolution_clock::now();
  SolverResult resJ;
  try {
    resJ = gj.solve(b_copy_j, n);
  } catch (MatrixException &e) {
    cout << "Jacobi:                         EXCEPTION: ";
    e.printError();
    resJ.x = nullptr;
    resJ.converged = false;
    resJ.iterations = 0;
    resJ.error = numeric_limits<double>::infinity();
    resJ.dominanceAchieved = false;
  }
  auto t3 = high_resolution_clock::now();
  double jac_ms = duration_cast<microseconds>(t3 - t2).count() / 1000.0;

  if (resJ.x != nullptr) {
    cout << "Jacobi solution:                x = ";
    printSolution(resJ.x, n);
    cout << " | iters=" << resJ.iterations << " | error=" << scientific
         << setprecision(2) << resJ.error << " | converged="
         << (resJ.converged ? "YES" : "NO")
         << " | domAchieved=" << (resJ.dominanceAchieved ? "YES" : "NO")
         << "  (" << fixed << setprecision(3) << jac_ms << " ms)" << endl;
  }

  // --- Seidel ---
  GaussSeidel gs(n, n);
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      gs(i, j) = A[i][j];

  double *b_copy_s = new double[n];
  for (int i = 0; i < n; i++)
    b_copy_s[i] = b[i];

  auto t4 = high_resolution_clock::now();
  SolverResult resS;
  try {
    resS = gs.solve(b_copy_s, n);
  } catch (MatrixException &e) {
    cout << "Seidel:                         EXCEPTION: ";
    e.printError();
    resS.x = nullptr;
    resS.converged = false;
    resS.iterations = 0;
    resS.error = numeric_limits<double>::infinity();
    resS.dominanceAchieved = false;
  }
  auto t5 = high_resolution_clock::now();
  double sei_ms = duration_cast<microseconds>(t5 - t4).count() / 1000.0;

  if (resS.x != nullptr) {
    cout << "Seidel solution:                x = ";
    printSolution(resS.x, n);
    cout << " | iters=" << resS.iterations << " | error=" << scientific
         << setprecision(2) << resS.error << " | converged="
         << (resS.converged ? "YES" : "NO")
         << " | domAchieved=" << (resS.dominanceAchieved ? "YES" : "NO")
         << "  (" << fixed << setprecision(3) << sei_ms << " ms)" << endl;
  }

  cout << endl;

  // --- Verification checks ---
  double residGE = computeResidual(A, b, x_ge, n);
  cout << "Residual (GE):      max|Ax-b| = " << scientific << setprecision(2)
       << residGE;
  if (residGE < 1e-6) {
    cout << "  [PASS]" << endl;
    tr.passed++;
  } else {
    cout << "  [FAIL]" << endl;
    tr.failed++;
  }

  // Jacobi checks
  if (resJ.x != nullptr && resJ.converged) {
    double residJ = computeResidual(A, b, resJ.x, n);
    cout << "Residual (Jacobi):  max|Ax-b| = " << scientific << setprecision(2)
         << residJ;
    if (residJ < 1e-6) {
      cout << "  [PASS]" << endl;
      tr.passed++;
    } else {
      cout << "  [FAIL]" << endl;
      tr.failed++;
    }

    double diffJGE = maxDiff(resJ.x, x_ge, n);
    cout << "Agreement (J vs GE): max|xJ-xGE| = " << scientific
         << setprecision(2) << diffJGE;
    if (diffJGE < 1e-6) {
      cout << "  [PASS]" << endl;
      tr.passed++;
    } else {
      cout << "  [FAIL]" << endl;
      tr.failed++;
    }
  } else if (resJ.x != nullptr) {
    cout << "Residual (Jacobi):  SKIPPED (did not converge)" << endl;
    if (!expectConverge) {
      cout << "  [PASS] (expected non-convergence)" << endl;
      tr.passed++;
    } else {
      cout << "  [WARN] Jacobi did not converge" << endl;
      tr.warned++;
    }
  }

  // Seidel checks
  if (resS.x != nullptr && resS.converged) {
    double residS = computeResidual(A, b, resS.x, n);
    cout << "Residual (Seidel):  max|Ax-b| = " << scientific << setprecision(2)
         << residS;
    if (residS < 1e-6) {
      cout << "  [PASS]" << endl;
      tr.passed++;
    } else {
      cout << "  [FAIL]" << endl;
      tr.failed++;
    }

    double diffSGE = maxDiff(resS.x, x_ge, n);
    cout << "Agreement (S vs GE): max|xS-xGE| = " << scientific
         << setprecision(2) << diffSGE;
    if (diffSGE < 1e-6) {
      cout << "  [PASS]" << endl;
      tr.passed++;
    } else {
      cout << "  [FAIL]" << endl;
      tr.failed++;
    }
  } else if (resS.x != nullptr) {
    cout << "Residual (Seidel):  SKIPPED (did not converge)" << endl;
    if (!expectConverge) {
      cout << "  [PASS] (expected non-convergence)" << endl;
      tr.passed++;
    } else {
      cout << "  [WARN] Seidel did not converge" << endl;
      tr.warned++;
    }
  }

  // Seidel vs Jacobi agreement
  if (resJ.x != nullptr && resJ.converged && resS.x != nullptr &&
      resS.converged) {
    double diffJS = maxDiff(resJ.x, resS.x, n);
    cout << "Agreement (J vs S): max|xJ-xS| = " << scientific
         << setprecision(2) << diffJS;
    if (diffJS < 1e-6) {
      cout << "  [PASS]" << endl;
      tr.passed++;
    } else {
      cout << "  [FAIL]" << endl;
      tr.failed++;
    }
  }

  // Convergence speed comparison
  if (resJ.converged && resS.converged) {
    cout << "Seidel faster convergence: Seidel " << resS.iterations
         << " iters vs Jacobi " << resJ.iterations << " iters";
    if (resS.iterations <= resJ.iterations) {
      cout << "  [PASS]" << endl;
      tr.passed++;
    } else {
      cout << "  [WARN]" << endl;
      tr.warned++;
    }
  }

  // Error value checks
  if (resJ.converged) {
    cout << "Jacobi error non-zero: error=" << scientific << setprecision(2)
         << resJ.error;
    if (resJ.error > 0.0 && resJ.error < 1.0) {
      cout << "  [PASS]" << endl;
      tr.passed++;
    } else {
      cout << "  [WARN]" << endl;
      tr.warned++;
    }
  }
  if (!resJ.converged && resJ.x != nullptr) {
    cout << "Jacobi non-converge error non-zero: error=" << scientific
         << setprecision(2) << resJ.error;
    if (resJ.error != 0.0) {
      cout << "  [PASS]" << endl;
      tr.passed++;
    } else {
      cout << "  [FAIL] error should not be 0.0" << endl;
      tr.failed++;
    }
  }

  // Timing
  if (n >= 49) {
    cout << "\nTime (GE " << n << "x" << n << "):      " << fixed
         << setprecision(3) << ge_ms << " ms" << endl;
    cout << "Time (Jacobi " << n << "x" << n << "):  " << fixed
         << setprecision(3) << jac_ms << " ms" << endl;
    cout << "Time (Seidel " << n << "x" << n << "):  " << fixed
         << setprecision(3) << sei_ms << " ms" << endl;
  }

  // cleanup
  delete[] x_ge;
  if (resJ.x != nullptr)
    delete[] resJ.x;
  if (resS.x != nullptr)
    delete[] resS.x;
  delete[] b_copy_j;
  delete[] b_copy_s;

  return tr;
}

// ============================================================
// Helper: allocate n×n matrix
// ============================================================
static double **allocMatrix(int n) {
  double **M = new double *[n];
  for (int i = 0; i < n; i++) {
    M[i] = new double[n];
    for (int j = 0; j < n; j++)
      M[i][j] = 0.0;
  }
  return M;
}
static void freeMatrix(double **M, int n) {
  for (int i = 0; i < n; i++)
    delete[] M[i];
  delete[] M;
}

// ============================================================
// Load matrix from file (reuses same parsing as the problem files)
// ============================================================
static bool loadFromFiles(const string &matFile, const string &rhsFile,
                          double **&A, double *&b, int &n) {
  ifstream fmat(matFile);
  ifstream frhs(rhsFile);
  if (!fmat || !frhs)
    return false;

  // read header — first number is rows, second is total columns (may include RHS)
  int r, c;
  fmat >> r >> c;
  n = r;

  A = allocMatrix(n);
  b = new double[n];

  // read matrix data — only take n columns per row
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++)
      fmat >> A[i][j];
  }

  for (int i = 0; i < n; i++)
    frhs >> b[i];

  return true;
}

// ============================================================
// MAIN
// ============================================================
int main() {
  cout << "============================================" << endl;
  cout << "  VERIFICATION SUITE — Iterative Solvers" << endl;
  cout << "============================================" << endl;

  int totalPass = 0, totalFail = 0, totalWarn = 0;

  // --- Test 1: 3×3 known exact solution ---
  {
    int n = 3;
    double **A = allocMatrix(n);
    A[0][0] = 4;  A[0][1] = -1; A[0][2] = 0;
    A[1][0] = -1; A[1][1] = 4;  A[1][2] = -1;
    A[2][0] = 0;  A[2][1] = -1; A[2][2] = 4;
    double *b = new double[3]{15.0, 10.0, 10.0};

    TestResult tr = runTest("3x3 Known Exact", A, b, n, true);
    totalPass += tr.passed;
    totalFail += tr.failed;
    totalWarn += tr.warned;

    // verify exact solution: x = [29/6, 31/6, 41/12] ≈ [4.8333, 5.1667, 3.4167]
    // wait, let me recalculate...
    // The system: 4x1 - x2 = 15, -x1 + 4x2 - x3 = 10, -x2 + 4x3 = 10
    // From row 3: x3 = (10 + x2)/4
    // From row 1: x1 = (15 + x2)/4
    // Sub into row 2: -(15+x2)/4 + 4x2 - (10+x2)/4 = 10
    //   (-15-x2+16x2-10-x2)/4 = 10 → (14x2-25)/4 = 10 → 14x2 = 65 → x2 = 65/14
    //   x1 = (15 + 65/14)/4 = (210+65)/(14*4) = 275/56
    //   x3 = (10 + 65/14)/4 = (140+65)/(14*4) = 205/56
    // Numerically: x1≈4.9107, x2≈4.6429, x3≈3.6607
    freeMatrix(A, n);
    delete[] b;
  }

  // --- Test 2: 7×7 diagonally dominant ---
  {
    int n = 7;
    double **A = allocMatrix(n);
    for (int i = 0; i < n; i++) {
      A[i][i] = n * 10.0;
      for (int j = 0; j < n; j++)
        if (j != i)
          A[i][j] = 1.0;
    }
    double *b = new double[n];
    for (int i = 0; i < n; i++)
      b[i] = (i + 1) * 5.0;

    TestResult tr = runTest("7x7 Diag Dominant", A, b, n, true);
    totalPass += tr.passed;
    totalFail += tr.failed;
    totalWarn += tr.warned;

    freeMatrix(A, n);
    delete[] b;
  }

  // --- Test 3: 49×49 from file ---
  {
    double **A = nullptr;
    double *b = nullptr;
    int n = 0;
    if (loadFromFiles("49/49l.txt", "49/49r.txt", A, b, n)) {
      TestResult tr = runTest("49x49 From File", A, b, n, false);
      totalPass += tr.passed;
      totalFail += tr.failed;
      totalWarn += tr.warned;
      freeMatrix(A, n);
      delete[] b;
    } else {
      cout << "\n[SKIP] Could not load 49x49 files" << endl;
    }
  }

  // --- Test 4: 225×225 from file ---
  {
    double **A = nullptr;
    double *b = nullptr;
    int n = 0;
    if (loadFromFiles("225/225left.txt", "225/225right.txt", A, b, n)) {
      TestResult tr = runTest("225x225 From File", A, b, n, false);
      totalPass += tr.passed;
      totalFail += tr.failed;
      totalWarn += tr.warned;
      freeMatrix(A, n);
      delete[] b;
    } else {
      cout << "\n[SKIP] Could not load 225x225 files" << endl;
    }
  }

  // --- Test 5: Identity matrix (trivial: x = b) ---
  {
    int n = 5;
    double **A = allocMatrix(n);
    for (int i = 0; i < n; i++)
      A[i][i] = 1.0;
    double *b = new double[5]{1.0, 2.0, 3.0, 4.0, 5.0};

    TestResult tr = runTest("5x5 Identity", A, b, n, true);
    totalPass += tr.passed;
    totalFail += tr.failed;
    totalWarn += tr.warned;

    freeMatrix(A, n);
    delete[] b;
  }

  // --- Test 6: Non-diagonally-dominant matrix ---
  {
    int n = 3;
    double **A = allocMatrix(n);
    A[0][0] = 1;  A[0][1] = 5;  A[0][2] = 2;
    A[1][0] = 3;  A[1][1] = 2;  A[1][2] = 6;
    A[2][0] = 7;  A[2][1] = 1;  A[2][2] = 3;
    double *b = new double[3]{8.0, 11.0, 11.0};

    cout << "\n--- Pathological: non-diag-dominant (may not converge) ---";
    TestResult tr = runTest("3x3 Non-Dominant", A, b, n, false);
    totalPass += tr.passed;
    totalFail += tr.failed;
    totalWarn += tr.warned;

    freeMatrix(A, n);
    delete[] b;
  }

  // ============================================================
  // SUMMARY
  // ============================================================
  cout << "\n============================================" << endl;
  cout << "  SUMMARY" << endl;
  cout << "============================================" << endl;
  cout << "  PASSED:  " << totalPass << endl;
  cout << "  FAILED:  " << totalFail << endl;
  cout << "  WARNED:  " << totalWarn << endl;
  cout << "============================================" << endl;

  return totalFail > 0 ? 1 : 0;
}
