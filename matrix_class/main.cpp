#include <iostream>
#include "include/Matrix.hpp"
#include "utils/Input.hpp"
#include "utils/Display.hpp"
using namespace std;


int main() {


    int rows1, cols1, rows2, cols2;
    cout << "Enter rows: ";    cin >> rows1;
    cout << "Enter cols: ";    cin >> cols1;

    Matrix mat1(rows1, cols1);
    cout << "\nMatrix 1:"<< endl;
    takeMatrixInput(mat1);

    cout << "Enter values for Matrix 2"<<endl;
    cout << "Enter rows: ";    cin >> rows2;
    cout << "Enter cols: ";    cin >> cols2;
    Matrix mat2(rows2, cols2);



    cout << "\nMatrix 2:" << endl;
    takeMatrixInput(mat2);

    showMatrix("Matrix 1:", mat1);
    showMatrix("Matrix 2:", mat2);

    cout << "\nADDITION:" << endl;
    try {
        Matrix added = mat1 + mat2;
        showMatrix("mat1 + mat2", added);
    }
    catch (MatrixError& e) {
        cout << "Error: " << e.what() << endl;
    }

    cout << "\nSUBTRACTION:" << endl;
    try {
        Matrix subbed = mat1 - mat2;
        showMatrix("mat1 - mat2", subbed);
    }
    catch (MatrixError& e) {
        cout << "Error: " << e.what() << endl;
    }

    cout << "\nASSIGNMENT:" << endl;
    Matrix mat3(rows1, cols1);
    mat3 = mat1;
    showMatrix("mat3", mat3);

    cout << "\nCOPY:" << endl;
    Matrix mat4(mat2);
    showMatrix("mat4:", mat4);

    cout << "\nGAUSSIAN ELIMINATION:" << endl;
    int n;
    cout << "Enter n: ";
    cin >> n;

    GaussianElimination gauss_ji(n);
    gauss_ji.takeInput();
    gauss_ji.compute();
    gauss_ji.showSolution();


    return 0;
}
