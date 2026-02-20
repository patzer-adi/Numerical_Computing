#include "Input.hpp"
#include <iostream>
using namespace std;


void takeMatrixInput(Matrix& m) {
    cout << "Enter values row by row:" << endl;
    for (int i = 0; i < m.getRows(); i++) {
        for (int j = 0; j < m.getCols(); j++) {
            cout << "  [" << i+1 << "][" << j+1 << "]: ";
            cin >> m.getData()[i][j];
        }
    }
}


void takeGaussInput(double** mat, int n) {
    cout << "\nEnter augmented matrix rows." << endl;
    for (int i = 0; i < n; i++) {
        cout << "Row " << i+1 << ": ";
        for (int j = 0; j <= n; j++) {
            cin >> mat[i][j];
        }
    }
}
