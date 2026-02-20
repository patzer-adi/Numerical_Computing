#include "Display.hpp"
#include <iostream>
using namespace std;


// Print a matrix with a title above it
void showMatrix(const string& label, Matrix& m) {
    cout << "\n" << label << ":" << endl;
    m.display();   // calls Matrix::display() — the virtual override
}


// Print x1, x2, x3... answers from Gaussian elimination
void showSolution(double* solution, int n) {
    cout << "\n===== SOLUTION =====" << endl;
    for (int i = 0; i < n; i++) {
        cout << "  x" << i+1 << " = " << solution[i] << endl;
    }
    cout << "====================" << endl;
}