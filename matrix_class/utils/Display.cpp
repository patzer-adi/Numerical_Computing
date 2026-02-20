#include "Display.hpp"
#include <iostream>
using namespace std;


void showMatrix(const string& label, Matrix& m) {
    cout << "\n" << label << ":" << endl;
    m.display();
}


void showSolution(double* solution, int n) {
    for (int i = 0; i < n; i++) {
        cout << "  x" << i+1 << " = " << solution[i] << endl;
    }
}
