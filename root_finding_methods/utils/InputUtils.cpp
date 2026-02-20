#include "InputUtils.hpp"
#include <iostream>
using namespace std;

double getTolerance() {
    double tol;
    cout << "Enter tolerance: ";
    cin >> tol;
    return tol;
}
