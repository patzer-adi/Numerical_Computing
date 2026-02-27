#include "include/GaussianElimination.hpp"
#include "include/LUDecomposition.hpp"
#include "include/Matrix.hpp"
#include "utils/Display.hpp"
#include "utils/Input.hpp"
#include <iostream>
using namespace std;

int main() {
  cout << "\n=== Matrix Operations Program ===\n\n";

  int choice;
  bool running = true;

  while (running) {
    cout << "\n=== Menu ===\n";
    cout << "1.  Add (A + B)\n";
    cout << "2.  Subtract (A - B)\n";
    cout << "3.  Multiply (A * B)\n";
    cout << "4.  Determinant\n";
    cout << "5.  Gaussian elimination (with pivoting)\n";
    cout << "6.  Gaussian elimination (without pivoting)\n";
    cout << "7.  LU - Doolittle\n";
    cout << "8.  LU - Crout\n";
    cout << "9.  LU - Cholesky\n";
    cout << "10. Exit\n";
    cout << "Enter choice: ";
    cin >> choice;

    try {
      switch (choice) {

      case 1: {
        cout << "\n--- Matrix A ---\n";
        Matrix A;
        getMatrixInput(A);

        cout << "\n--- Matrix B ---\n";
        Matrix B;
        getMatrixInput(B);

        Matrix C = A + B;
        cout << "\nResult (A + B):\n";
        C.display();
        break;
      }

      case 2: {
        cout << "\n--- Matrix A ---\n";
        Matrix A;
        getMatrixInput(A);

        cout << "\n--- Matrix B ---\n";
        Matrix B;
        getMatrixInput(B);

        Matrix C = A - B;
        cout << "\nResult (A - B):\n";
        C.display();
        break;
      }

      case 3: {
        cout << "\n--- Matrix A ---\n";
        Matrix A;
        getMatrixInput(A);

        cout << "\n--- Matrix B ---\n";
        Matrix B;
        getMatrixInput(B);

        Matrix C = A * B;
        cout << "\nResult (A * B):\n";
        C.display();
        break;
      }

      case 4: {
        cout << "\n--- Enter square matrix ---\n";
        Matrix A;
        getMatrixInput(A);
        cout << "\nDeterminant = " << A.determinant() << endl;
        break;
      }

      case 5: {
        GaussianElimination ge;
        solveGaussian(ge, true);
        break;
      }

      case 6: {
        GaussianElimination ge;
        solveGaussian(ge, false);
        break;
      }

      case 7: {
        Doolittle d;
        solveLU(d);
        break;
      }

      case 8: {
        Crout c;
        solveLU(c);
        break;
      }

      case 9: {
        Cholesky ch;
        solveLU(ch);
        break;
      }

      case 10:
        cout << "\nbye bye!\n";
        running = false;
        break;

      default:
        cout << "\ninvalid choice... try again\n";
        break;
      }
    } catch (MatrixException &e) {
      e.printError();
    }
  }

  return 0;
}
