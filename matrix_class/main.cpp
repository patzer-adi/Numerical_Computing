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
    cout << "1. Add two matrices\n";
    cout << "2. Subtract two matrices\n";
    cout << "3. Multiply two matrices\n";
    cout << "4. Calculate determinant\n";
    cout << "5. Gaussian elimination with pivoting\n";
    cout << "6. Gaussian elimination without pivoting\n";
    cout << "7. LU Decomposition (Doolittle)\n";
    cout << "8. LU Decomposition (Crout)\n";
    cout << "9. LU Decomposition (Cholesky)\n";
    cout << "10. Exit\n";
    cout << "Enter choice: ";
    cin >> choice;

    try {
      switch (choice) {

      case 1: {
        // addition
        cout << "\n--- Matrix A ---\n";
        double **dataA = nullptr;
        int rA, cA;
        getMatrixInput(dataA, rA, cA);

        cout << "\n--- Matrix B ---\n";
        double **dataB = nullptr;
        int rB, cB;
        getMatrixInput(dataB, rB, cB);

        Matrix A(rA, cA);
        for (int i = 0; i < rA; i++)
          for (int j = 0; j < cA; j++)
            A.setData(i, j, dataA[i][j]);

        Matrix B(rB, cB);
        for (int i = 0; i < rB; i++)
          for (int j = 0; j < cB; j++)
            B.setData(i, j, dataB[i][j]);

        Matrix C = A.add(B);
        cout << "\nResult (A + B):\n";
        C.display();

        for (int i = 0; i < rA; i++)
          delete[] dataA[i];
        delete[] dataA;
        for (int i = 0; i < rB; i++)
          delete[] dataB[i];
        delete[] dataB;
        break;
      }

      case 2: {
        // subtraction
        cout << "\n--- Matrix A ---\n";
        double **dataA = nullptr;
        int rA, cA;
        getMatrixInput(dataA, rA, cA);

        cout << "\n--- Matrix B ---\n";
        double **dataB = nullptr;
        int rB, cB;
        getMatrixInput(dataB, rB, cB);

        Matrix A(rA, cA);
        for (int i = 0; i < rA; i++)
          for (int j = 0; j < cA; j++)
            A.setData(i, j, dataA[i][j]);

        Matrix B(rB, cB);
        for (int i = 0; i < rB; i++)
          for (int j = 0; j < cB; j++)
            B.setData(i, j, dataB[i][j]);

        Matrix C = A.subtract(B);
        cout << "\nResult (A - B):\n";
        C.display();

        for (int i = 0; i < rA; i++)
          delete[] dataA[i];
        delete[] dataA;
        for (int i = 0; i < rB; i++)
          delete[] dataB[i];
        delete[] dataB;
        break;
      }

      case 3: {
        // multiplication
        cout << "\n--- Matrix A ---\n";
        double **dataA = nullptr;
        int rA, cA;
        getMatrixInput(dataA, rA, cA);

        cout << "\n--- Matrix B ---\n";
        double **dataB = nullptr;
        int rB, cB;
        getMatrixInput(dataB, rB, cB);

        Matrix A(rA, cA);
        for (int i = 0; i < rA; i++)
          for (int j = 0; j < cA; j++)
            A.setData(i, j, dataA[i][j]);

        Matrix B(rB, cB);
        for (int i = 0; i < rB; i++)
          for (int j = 0; j < cB; j++)
            B.setData(i, j, dataB[i][j]);

        Matrix C = A.multiply(B);
        cout << "\nResult (A * B):\n";
        C.display();

        for (int i = 0; i < rA; i++)
          delete[] dataA[i];
        delete[] dataA;
        for (int i = 0; i < rB; i++)
          delete[] dataB[i];
        delete[] dataB;
        break;
      }

      case 4: {
        // determinant
        cout << "\n--- Enter square matrix ---\n";
        double **dataA = nullptr;
        int rA, cA;
        getMatrixInput(dataA, rA, cA);

        Matrix A(rA, cA);
        for (int i = 0; i < rA; i++)
          for (int j = 0; j < cA; j++)
            A.setData(i, j, dataA[i][j]);

        double det = A.determinant();
        cout << "\nDeterminant = " << det << endl;

        for (int i = 0; i < rA; i++)
          delete[] dataA[i];
        delete[] dataA;
        break;
      }

      case 5: {
        // gaussian with pivoting
        double **A = nullptr;
        double *b = nullptr;
        int n;
        getSystemInput(A, b, n);

        GaussianElimination ge(n, n);
        for (int i = 0; i < n; i++)
          for (int j = 0; j < n; j++)
            ge.setData(i, j, A[i][j]);

        double *x = ge.solveWithPivoting(b, n);
        displaySolution(x, n);

        // ask if user wants to save
        cout << "Save solution to file? (1=yes, 0=no): ";
        int save;
        cin >> save;
        if (save == 1) {
          string outFile;
          cout << "Enter output filename: ";
          cin >> outFile;
          writeSolutionToFile(x, n, outFile);
        }

        delete[] x;
        delete[] b;
        for (int i = 0; i < n; i++)
          delete[] A[i];
        delete[] A;
        break;
      }

      case 6: {
        // gaussian without pivoting
        double **A = nullptr;
        double *b = nullptr;
        int n;
        getSystemInput(A, b, n);

        GaussianElimination ge(n, n);
        for (int i = 0; i < n; i++)
          for (int j = 0; j < n; j++)
            ge.setData(i, j, A[i][j]);

        double *x = ge.solveWithoutPivoting(b, n);
        displaySolution(x, n);

        cout << "Save solution to file? (1=yes, 0=no): ";
        int save;
        cin >> save;
        if (save == 1) {
          string outFile;
          cout << "Enter output filename: ";
          cin >> outFile;
          writeSolutionToFile(x, n, outFile);
        }

        delete[] x;
        delete[] b;
        for (int i = 0; i < n; i++)
          delete[] A[i];
        delete[] A;
        break;
      }

      case 7:
        cout << "\nDoolittle LU Decomposition - not implemented yet, coming "
                "soon!\n";
        break;

      case 8:
        cout << "\nCrout LU Decomposition - not implemented yet, coming soon! "
                "\n";
        break;

      case 9:
        cout << "\nCholesky LU Decomposition - not implemented yet, coming "
                "soon!\n";
        break;

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
