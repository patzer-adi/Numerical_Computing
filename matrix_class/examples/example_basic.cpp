// example_basic.cpp — basic matrix operations demo
// Shows: create, add, subtract, multiply, scalar mul, transpose, det, inverse
//
// COMPILE:
//   g++ -std=c++11 -o example_basic examples/example_basic.cpp \
//       src/Matrix.cpp src/MatrixException.cpp src/MatrixOperations.cpp
//
// RUN:
//   ./example_basic

#include "../include/Matrix.hpp"
#include <iostream>
using namespace std;

int main() {
  cout << "=== Basic Matrix Operations Demo ===" << endl;

  // create two 2x2 matrices
  Matrix A(2, 2);
  A.setData(0, 0, 1);
  A.setData(0, 1, 2);
  A.setData(1, 0, 3);
  A.setData(1, 1, 4);

  Matrix B(2, 2);
  B.setData(0, 0, 5);
  B.setData(0, 1, 6);
  B.setData(1, 0, 7);
  B.setData(1, 1, 8);

  cout << "\nA:" << endl;
  A.display();

  cout << "\nB:" << endl;
  B.display();

  // addition
  cout << "\nA + B:" << endl;
  Matrix C = A + B;
  C.display();

  // subtraction
  cout << "\nA - B:" << endl;
  Matrix D = A - B;
  D.display();

  // multiplication
  cout << "\nA * B:" << endl;
  Matrix E = A * B;
  E.display();

  // scalar multiply
  cout << "\nA * 3:" << endl;
  Matrix F = A * 3.0;
  F.display();

  // transpose
  cout << "\nTranspose of A:" << endl;
  Matrix T = A.transpose();
  T.display();

  // determinant
  cout << "\ndet(A) = " << A.determinant() << endl;

  // inverse
  cout << "\nInverse of A:" << endl;
  Matrix inv = A.inverse();
  inv.display();

  // verify: A * A^(-1) should be identity
  cout << "\nA * A^(-1) (should be identity):" << endl;
  Matrix I = A * inv;
  I.display();

  cout << "\nDone!" << endl;
  return 0;
}
