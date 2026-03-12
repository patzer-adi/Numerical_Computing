// ============================================
// example_usage.cpp — Numerical Computing Library Demo
// ============================================
// Demonstrates using the unified library:
//   - Matrix operations
//   - Complex arithmetic
//   - Matrix property checks
//
// Build:
//   make example
//   ./examples/example_usage
// ============================================

#include "numcomp.hpp"
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    cout << "\n========================================" << endl;
    cout << "  Numerical Computing Library — Demo" << endl;
    cout << "========================================\n" << endl;

    // ─── 1. Matrix Arithmetic ────────────────
    cout << "--- 1. Matrix Arithmetic ---\n" << endl;

    Matrix A(3, 3);
    A(0,0) = 1;  A(0,1) = 2;  A(0,2) = 3;
    A(1,0) = 4;  A(1,1) = 5;  A(1,2) = 6;
    A(2,0) = 7;  A(2,1) = 8;  A(2,2) = 10;

    cout << "Matrix A:" << endl;
    cout << A;

    Matrix B = A.transpose();
    cout << "Transpose of A:" << endl;
    cout << B;

    Matrix C = A + B;
    cout << "A + A^T (symmetric result):" << endl;
    cout << C;

    // ─── 2. Determinant & Inverse ────────────
    cout << "--- 2. Determinant & Inverse ---\n" << endl;

    double det = A.determinant();
    cout << "det(A) = " << det << endl;

    if (fabs(det) > 1e-10) {
        Matrix inv = A.inverse();
        cout << "\nInverse of A:" << endl;
        cout << inv;

        Matrix identity = A * inv;
        cout << "A * A^(-1) (should be identity):" << endl;
        cout << identity;
    }

    // ─── 3. Matrix Property Checks ───────────
    cout << "--- 3. Matrix Property Checks ---\n" << endl;

    cout << "A is square?              " << (A.isSquare() ? "YES" : "NO") << endl;
    cout << "A is symmetric?           " << (A.isSymmetric() ? "YES" : "NO") << endl;
    cout << "A is identity?            " << (A.isIdentity() ? "YES" : "NO") << endl;
    cout << "A is null?                " << (A.isNull() ? "YES" : "NO") << endl;
    cout << "A is diagonal?            " << (A.isDiagonal() ? "YES" : "NO") << endl;
    cout << "A is diag. dominant?      " << (A.isDiagonallyDominant() ? "YES" : "NO") << endl;
    cout << "A+A^T is symmetric?       " << (C.isSymmetric() ? "YES" : "NO") << endl;

    // ─── 4. Scalar Multiplication ────────────
    cout << "\n--- 4. Scalar Multiplication ---\n" << endl;

    Matrix S = A * 2.0;
    cout << "A * 2.0:" << endl;
    cout << S;

    // ─── 5. Matrix Equality ──────────────────
    cout << "--- 5. Matrix Equality ---\n" << endl;

    Matrix D = A;
    cout << "A == copy(A)?  " << (A == D ? "YES" : "NO") << endl;
    cout << "A == A*2?      " << (A == S ? "YES" : "NO") << endl;

    // ─── 6. Complex Numbers ──────────────────
    cout << "\n--- 6. Complex Numbers ---\n" << endl;

    Complex c1(3.0, 4.0);
    Complex c2(1.0, -2.0);

    cout << "c1 = ";  c1.display();
    cout << "c2 = ";  c2.display();

    Complex sum  = c1 + c2;
    Complex diff = c1 - c2;
    Complex prod = c1 * c2;
    Complex quot = c1 / c2;
    Complex conj = c1.conjugate();

    cout << "\nc1 + c2 = ";  sum.display();
    cout << "c1 - c2 = ";  diff.display();
    cout << "c1 * c2 = ";  prod.display();
    cout << "c1 / c2 = ";  quot.display();
    cout << "conj(c1) = "; conj.display();
    cout << "|c1| = " << c1.Norm() << endl;

    // ─── 7. Make Diagonally Dominant ─────────
    cout << "\n--- 7. Make Diagonally Dominant ---\n" << endl;

    Matrix E(3, 3);
    E(0,0) = 1; E(0,1) = 2; E(0,2) = 3;
    E(1,0) = 4; E(1,1) = 5; E(1,2) = 6;
    E(2,0) = 9; E(2,1) = 1; E(2,2) = 1;

    cout << "Original matrix E:" << endl;
    cout << E;
    cout << "isDiagonallyDominant? " << (E.isDiagonallyDominant() ? "YES" : "NO") << endl;

    cout << "\nAttempting to make diagonally dominant..." << endl;
    Matrix F = E.makeDiagonallyDominant();
    cout << "Result:" << endl;
    cout << F;
    cout << "isDiagonallyDominant? " << (F.isDiagonallyDominant() ? "YES" : "NO") << endl;

    // ─── Done ────────────────────────────────
    cout << "\n========================================" << endl;
    cout << "  All demos completed successfully!" << endl;
    cout << "========================================\n" << endl;

    return 0;
}
