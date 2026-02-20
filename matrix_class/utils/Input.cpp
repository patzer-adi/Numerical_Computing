#include "Input.hpp"
#include <iostream>
using namespace std;


// Ask the user to type values into a matrix cell by cell
void takeMatrixInput(Matrix& m) {
    cout << "Enter values row by row:" << endl;
    for (int i = 0; i < m.getRows(); i++) {
        for (int j = 0; j < m.getCols(); j++) {
            cout << "  [" << i+1 << "][" << j+1 << "]: ";
            cin >> m.getData()[i][j];
        }
    }
}


// Ask the user to type the augmented matrix for Gaussian elimination
// Each row looks like:  a  b  c  (for ax + by = c)
void takeGaussInput(double** mat, int n) {
    cout << "\nEnter augmented matrix rows." << endl;
    cout << "For  ax + by = c,  type:  a  b  c" << endl;
    for (int i = 0; i < n; i++) {
        cout << "Row " << i+1 << ": ";
        for (int j = 0; j <= n; j++) {
            cin >> mat[i][j];
        }
    }
}