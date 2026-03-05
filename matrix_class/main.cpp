#include "include/GaussJacobi.hpp"
#include "include/GaussianElimination.hpp"
#include "include/LUDecomposition.hpp"
#include "include/Matrix.hpp"
#include "utils/Display.hpp"
#include "utils/Input.hpp"
#include <iostream>

#ifdef USE_CUDA
#include "cuda/include/gpu_backend.cuh"
#include "cuda/include/gpu_dispatch.cuh"
#endif

using namespace std;

int main() {
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
    cout << "11. Transpose" << endl;
    cout << "12. Scalar Multiply" << endl;
    cout << "13. Inverse" << endl;
    cout << "14. Minor Matrix" << endl;
    cout << "15. Cofactor" << endl;
    cout << "16. Adjoint" << endl;
    cout << "17. Exit" << endl;
    cout << "Enter choice: ";
    cin >> choice;

    try {
      switch (choice) {

      case 1: {
        cout << "\n--- Matrix A ---" << endl;
        Matrix A;
        getMatrixInput(A);

        cout << "\n--- Matrix B ---" << endl;
        Matrix B;
        getMatrixInput(B);

        Matrix C = A + B;
        cout << "\nResult (A + B):" << endl;
        C.display();
        break;
      }

      case 2: {
        cout << "\n--- Matrix A ---" << endl;
        Matrix A;
        getMatrixInput(A);

        cout << "\n--- Matrix B ---" << endl;
        Matrix B;
        getMatrixInput(B);

        Matrix C = A - B;
        cout << "\nResult (A - B):" << endl;
        C.display();
        break;
      }

      case 3: {
        cout << "\n--- Matrix A ---" << endl;
        Matrix A;
        getMatrixInput(A);

        cout << "\n--- Matrix B ---" << endl;
        Matrix B;
        getMatrixInput(B);

        Matrix C = A * B;
        cout << "\nResult (A * B):" << endl;
        C.display();
        break;
      }

      case 4: {
        cout << "\n--- Enter square matrix ---" << endl;
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

      case 10: {
        GaussJacobi gj;
        solveIterative(gj);
        break;
      }

      case 11: {
        cout << "\n--- Enter matrix to transpose ---" << endl;
        Matrix A;
        getMatrixInput(A);

        Matrix T = A.transpose();
        cout << "\nTranspose:" << endl;
        T.display();
        break;
      }

      case 12: {
        cout << "\n--- Enter matrix ---" << endl;
        Matrix A;
        getMatrixInput(A);

        double scalar;
        cout << "Enter scalar value: ";
        cin >> scalar;

        Matrix S = A * scalar;
        cout << "\nResult (A * " << scalar << "):" << endl;
        S.display();
        break;
      }

      case 13: {
        cout << "\n--- Enter square matrix ---" << endl;
        Matrix A;
        getMatrixInput(A);

        Matrix inv = A.inverse();
        cout << "\nInverse:" << endl;
        inv.display();
        break;
      }

      case 14: {
        cout << "\n--- Enter square matrix ---" << endl;
        Matrix A;
        getMatrixInput(A);

        int r, c;
        cout << "Enter row to remove (0-indexed): ";
        cin >> r;
        cout << "Enter col to remove (0-indexed): ";
        cin >> c;

        Matrix M = A.minorMatrix(r, c);
        cout << "\nMinor Matrix (removed row " << r << ", col " << c
             << "):" << endl;
        M.display();
        break;
      }

      case 15: {
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
        break;
      }

      case 16: {
        cout << "\n--- Enter square matrix ---" << endl;
        Matrix A;
        getMatrixInput(A);

        Matrix adj = A.adjoint();
        cout << "\nAdjoint:" << endl;
        adj.display();
        break;
      }

      case 17:
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

  return 0;
}
