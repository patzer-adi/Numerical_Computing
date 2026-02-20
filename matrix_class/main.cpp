#include <iostream>
#include "include/Matrix.hpp"
#include "utils/Input.hpp"
#include "utils/Display.hpp"
using namespace std;


int main() {


    int rows, cols;
    cout << "Enter rows: ";    cin >> rows;
    cout << "Enter cols: ";    cin >> cols;

    Matrix mat1(rows, cols);
    Matrix mat2(rows, cols);

    cout << "\nMatrix 1 —" << endl;
    takeMatrixInput(mat1);

    cout << "\nMatrix 2 —" << endl;
    takeMatrixInput(mat2);

    showMatrix("Matrix 1", mat1);
    showMatrix("Matrix 2", mat2);

    // Addition
    cout << "\nADDITION:" << endl;
    try {
        Matrix added = mat1 + mat2;
        showMatrix("mat1 + mat2", added);
    }
    catch (MatrixError& e) {
        cout << "Error: " << e.what() << endl;
    }


    // Subtraction
    cout << "\nSUBTRACTION:" << endl;
    try {
        Matrix subbed = mat1 - mat2;
        showMatrix("mat1 - mat2", subbed);
    }
    catch (MatrixError& e) {
        cout << "Error: " << e.what() << endl;
    }

    cout << "\nASSIGNMENT (mat3 = mat1):" << endl;
    Matrix mat3(rows, cols);
    mat3 = mat1;
    showMatrix("mat3 (copy of mat1)", mat3);


    cout << "\nGAUSSIAN ELIMINATION:" << endl;
    int n;
    cout << "Enter n: ";
    cin >> n;

    GaussianElimination gauss(n);
    gauss.takeInput();
    gauss.compute();
    gauss.showSolution();


    cout << "\n=== Done! ===\n" << endl;
    return 0;
}
